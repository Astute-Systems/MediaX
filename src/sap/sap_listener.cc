//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief Session Announcement Protocol (SDP) implementation for listening to announcements of stream data. The SAP
/// packet contains the Session Description Protocol (SDP).
///
/// \file sap_listener.h
///

#include "sap/sap_listener.h"

#include <fcntl.h>  // for fcntl(), F_GETFL, F_SETFL, O_NONBLOCK

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "glog/logging.h"
#include "rtp/rtp_internal_types.h"
#include "rtp/rtp_utils.h"
#include "sap/sap_utils.h"

/// The Session Announcment Protocol (SAP)/ Session Description Protocol (SDP) namespace
namespace mediax::sap {

bool SapListener::running_ = false;

SapListener::SapListener() {
  if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Set the socket to non-blocking mode
#ifdef _WIN32
  u_long mode = 1;
  if (ioctlsocket(sockfd_, FIONBIO, &mode) != NO_ERROR) {
    perror("ioctlsocket FIONBIO failed");
    exit(EXIT_FAILURE);
  }
#else
  int flags = fcntl(sockfd_, F_GETFL, 0);
  if (flags == -1) {
    perror("fcntl F_GETFL failed");
    exit(EXIT_FAILURE);
  }

  if (fcntl(sockfd_, F_SETFL, flags | O_NONBLOCK) == -1) {
    perror("fcntl F_SETFL O_NONBLOCK failed");
    exit(EXIT_FAILURE);
  }
#endif

  memset(&multicast_addr_, 0, sizeof(multicast_addr_));
  multicast_addr_.sin_family = AF_INET;
  multicast_addr_.sin_port = htons(mediax::rtp::kSapPort);
  multicast_addr_.sin_addr.s_addr = htonl(INADDR_ANY);

  // join the multicast group
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(mediax::rtp::kIpaddr);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (const auto *byte = reinterpret_cast<std::byte *>(&mreq);
      setsockopt(sockfd_, IPPROTO_IP, IP_ADD_MEMBERSHIP, byte, sizeof(mreq)) < 0) {
    perror("setsockopt IP_ADD_MEMBERSHIP");
  }

  // bind socket to port
  if (bind(sockfd_, (struct sockaddr *)&multicast_addr_, sizeof(multicast_addr_)) == -1) {
    std::cout << "SAPListener() " << std::string(strerror(errno)) << " " << mediax::rtp::kIpaddr << ":"
              << mediax::rtp::kSapPort << "\n";
    exit(-1);
  }
}

SapListener::~SapListener() { close(sockfd_); }

std::unique_ptr<SapListener> SapListener::singleton_;
SapListener &SapListener::GetInstance() {
  if (!singleton_) singleton_ = std::make_unique<SapListener>();
  return *singleton_;
}

void SapListener::SapListenerThread(SapListener *sap) {
  struct timeval read_timeout;
  read_timeout.tv_sec = 0;
  read_timeout.tv_usec = 10;
  setsockopt(sap->sockfd_, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

  DLOG(INFO) << "SAP packet received " << mediax::rtp::kIpaddr << ":" << mediax::rtp::kSapPort;

  while (running_) {
    ssize_t bytes = 0;
    if (bytes = recvfrom(sap->sockfd_, sap->udpdata_.data(), mediax::rtp::kMaxUdpData, 0, nullptr, nullptr);
        bytes <= 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
      continue;
    }

    // Process SAP here
    sap->SapStore(&sap->udpdata_, static_cast<uint32_t>(bytes));
  }
}

void SapListener::RegisterSapListener(std::string_view session_name, const SapCallback &callback, uint8_t *data) {
  data_ = data;
  DLOG(INFO) << "Register SAP listener with session-name=" << session_name;

  callbacks_[std::string(session_name)] = callback;
}

bool SapListener::GetStreamInformation(std::string_view session_name,
                                       mediax::rtp::StreamInformation *stream_information) const {
  if (auto it = announcements_.find(std::string(session_name)); it != announcements_.end()) {
    stream_information->hostname = it->second.ip_address;
    stream_information->port = it->second.port;
    stream_information->height = it->second.height;
    stream_information->width = it->second.width;
    stream_information->framerate = it->second.framerate;
    stream_information->encoding = SamplingToColourspaceType(it->second.sampling, it->second.bits_per_pixel);
    return true;
  }
  return false;
}

void SapListener::Start() {
  if (running_) return;
  running_ = true;
  thread_ = std::thread(SapListenerThread, this);
}

void SapListener::Stop() {
  running_ = false;
  if (thread_.joinable()) thread_.join();
}

SdpTypeEnum SapListener::GetType(const std::string_view &line) const {
  if (line.substr(0, 2).compare("v=") == 0) return SdpTypeEnum::kProtocolVersion;
  if (line.substr(0, 2).compare("o=") == 0) return SdpTypeEnum::kOriginatorSessionIdentifier;
  if (line.substr(0, 2).compare("s=") == 0) return SdpTypeEnum::kSessionName;
  if (line.substr(0, 2).compare("c=") == 0) return SdpTypeEnum::kConnectionInformation;
  if (line.substr(0, 2).compare("t=") == 0) return SdpTypeEnum::kTimeSessionActive;
  if (line.substr(0, 2).compare("m=") == 0) return SdpTypeEnum::kMediaNameAndTransportAddress;
  if (line.substr(0, 2).compare("a=") == 0) return SdpTypeEnum::kSessionAttribute;
  return SdpTypeEnum::kUnknown;
}

std::map<std::string, std::string, std::less<>> SapListener::ParseAttributes1(const std::string_view &line) const {
  auto line2 = std::string(line);
  std::map<std::string, std::string, std::less<>> attributes;

  // If line does not contain an equals sign just assign the whole line to the key
  if (line.find("=") == std::string::npos) {
    std::string line2 = std::string(line);
    attributes[line2] = "";
    return attributes;
  }
  // Split string after first space
  std::string key = line2.substr(0, line.find(" "));
  std::string value = line2.substr(line.find(" ") + 1);
  attributes[key] = value;
  return attributes;
}

std::map<std::string, std::string, std::less<>> SapListener::ParseAttributes(const std::string_view &line) const {
  std::map<std::string, std::string, std::less<>> attributes;
  std::string key;
  std::string value;
  bool attribute_name = true;

  // If line does not contain an equals sign just assign the whole line to the key
  if (line.find("=") == std::string::npos) {
    key = line;
    value = "";
    attributes[key] = value;
    return attributes;
  }

  // Step through characters
  for (char c : line) {
    if (c == ' ') {
      attribute_name = true;
      attributes[key] = value;
      // Clear key nd value
      key.clear();
      value.clear();
      continue;
    }
    if (attribute_name) {
      if (c != '=') {
        key += c;
      } else {
        attribute_name = false;
      }
    } else {
      // attribute_value
      if (c != ';') value += c;
    }
  }
  attributes[key] = value;

  return attributes;
}

bool SapListener::SapStore(std::array<uint8_t, mediax::rtp::kMaxUdpData> *rawdata, uint32_t size) {
  if (size < 8) {
    DLOG(ERROR) << "SAP packet too small";
    return false;
  }
  std::map<std::string, std::string, std::less<>> attributes_map;
  auto source = reinterpret_cast<uint32_t *>(&rawdata->at(4));

  SdpMessage sdp;
  // Check deletion flag
  // Print the first byte in hex
  if (rawdata->at(0) & 0x04) {
    // Deletion flag set
    sdp.deleted = true;
  } else {
    sdp.deleted = false;
  }
  // Find 'v=' marking the start of the SDP message in the sdp_text

  sdp.sdp_text = std::string(reinterpret_cast<char *>(&rawdata->at(8)), size - 8);
  std::size_t v_pos = sdp.sdp_text.find("v=");
  // Strip up to position of 'v='
  sdp.sdp_text = sdp.sdp_text.substr(v_pos);
  sdp.sdp_text.push_back('\0');
  // convert to string IP address
  struct in_addr addr;
  addr.s_addr = htonl(*source);
  EndianSwap32(&addr.s_addr, 1);
  sdp.ip_address_source = inet_ntoa(addr);
  // Loop through lines
  // Convert string to istringstream
  std::istringstream iss(sdp.sdp_text);
  std::string line;

  while (std::getline(iss, line)) {
    line.erase(remove(line.begin(), line.end(), '\r'), line.end());
    line.erase(remove(line.begin(), line.end(), '\n'), line.end());

    // Get the SDP type for this line
    SdpTypeEnum line_type = GetType(line);

    if (line_type == SdpTypeEnum::kUnknown) continue;

    /// Remove the first two characters now we know the type
    line = line.substr(2);

    switch (line_type) {
      case SdpTypeEnum::kProtocolVersion:
        sdp.protocol_version = std::stoi(line);
        break;
      case SdpTypeEnum::kOriginatorSessionIdentifier: {
        size_t lastSpace = line.find_last_of(" ");
        // Extract last word using substr
        sdp.ip_address_source = line.substr(lastSpace + 1);
        size_t pos = sdp.ip_address_source.find('/');  // Find position of the first forward slash
        if (pos != std::string::npos) {                // If a forward slash is found
          sdp.ip_address_source =
              sdp.ip_address_source.substr(0, pos);  // Get substring from start to the position of the forward slash
        }
      } break;
      case SdpTypeEnum::kSessionName:
        sdp.session_name = line;
        break;
      case SdpTypeEnum::kSessionInformation:
        break;
      case SdpTypeEnum::kUriOfDescription:
        break;
      case SdpTypeEnum::kEmailAddress:
        break;
      case SdpTypeEnum::kPhoneNumber:
        break;
      case SdpTypeEnum::kConnectionInformation: {
        size_t pos = line.find("IP4");            // Find position of "IP4" in the string
        if (pos != std::string::npos) {           // If "IP4" is found
          sdp.ip_address = line.substr(pos + 4);  // Get substring from position of "IP4" to the end of the string
          pos = sdp.ip_address.find("/");         // Find position of the forward slash
          if (pos != std::string::npos) {         // If a forward slash is found
            sdp.ip_address =
                sdp.ip_address.substr(0, pos);  // Get substring from start to the position of the forward slash
          }
        }
      } break;
      case SdpTypeEnum::kBandwidthInformation:
        break;
      case SdpTypeEnum::kSessionAttribute: {
        std::map<std::string, std::string, std::less<>> attributes_map_more = ParseAttributes1(line);
        attributes_map.insert(attributes_map_more.begin(), attributes_map_more.end());
      } break;
      case SdpTypeEnum::kTimeSessionActive:
        break;
      case SdpTypeEnum::kMediaNameAndTransportAddress: {
        // Get index of first space character
        size_t firstSpace = line.find(" ");
        // Extract second word using substr
        std::string secondWord = line.substr(firstSpace + 1);
        secondWord = secondWord.substr(0, secondWord.find(" "));
        sdp.port = std::stoi(secondWord);
      } break;
      case SdpTypeEnum::kMediaTitle:
        break;
      case SdpTypeEnum::KConnectionInformation:
        break;
      default:
        break;
    }
  }

  std::map<std::string, std::string, std::less<>> attributes_map_fmtp;
  attributes_map_fmtp = ParseAttributes(attributes_map["fmtp:103"]);
  attributes_map.insert(attributes_map_fmtp.begin(), attributes_map_fmtp.end());
  attributes_map_fmtp = ParseAttributes(attributes_map["fmtp:96"]);
  attributes_map.insert(attributes_map_fmtp.begin(), attributes_map_fmtp.end());

  // Iterate over the attributes_map and split key on :
  for (const auto &[key, value] : attributes_map) {
    // If key contains a :
    if (key.find(":") == std::string::npos) continue;
    std::string key2 = key.substr(0, key.find(":"));
    std::string value2 = key.substr(key.find(":") + 1);
    attributes_map[key2] = value2;
  }

  try {
    sdp.height = std::stoi(attributes_map["height"]);
    sdp.width = std::stoi(attributes_map["width"]);
    sdp.framerate = std::stoi(attributes_map["framerate"]);
    sdp.bits_per_pixel = std::stoi(attributes_map["depth"]);
    sdp.sampling = attributes_map["sampling"];
  } catch (const std::invalid_argument &e [[maybe_unused]]) {
    DLOG(ERROR) << "Invalid argument in SAP message. SDP text = " << sdp.sdp_text;
    sdp.sampling = attributes_map["profile-level-id"];
    if (sdp.sampling == "42A01E") {
      sdp.sampling = "H264";
    }
  }
  if (sdp.sampling.empty()) sdp.sampling = "jpeg2000";

  DLOG(INFO) << "Store " << sdp.session_name << " " << sdp.ip_address << ":" << sdp.port << " " << sdp.height << "x"
             << sdp.width << " " << sdp.framerate << "fps " << sdp.sampling;
  announcements_[sdp.session_name] = sdp;

  // Check the callbacks
  for (const auto &[name, callback] : callbacks_) {
    if (name == sdp.session_name) {
      // We have a match, hit the callback
      callback(&sdp, nullptr);
    }
    // Find the get all callbacks i.e. no name set
    if (name.empty()) {
      callback(&sdp, data_);
    }
  }
  return true;
}

const std::map<std::string, SdpMessage, std::less<>> &SapListener::GetSapAnnouncements() const {
  return announcements_;
}

}  // namespace mediax::sap
