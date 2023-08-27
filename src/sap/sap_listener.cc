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

#include "rtp/rtp_utils.h"

namespace mediax::sap {

bool SAPListener::running_ = false;

SAPListener::SAPListener() {
  if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }  // namespace sap

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
  multicast_addr_.sin_port = htons(kPort);
  multicast_addr_.sin_addr.s_addr = htonl(INADDR_ANY);

  // join the multicast group
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(kIpaddr);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(sockfd_, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char *>(&mreq), sizeof(mreq)) < 0) {
    perror("setsockopt IP_ADD_MEMBERSHIP");
    exit(EXIT_FAILURE);
  }

  // bind socket to port
  if (bind(sockfd_, (struct sockaddr *)&multicast_addr_, sizeof(multicast_addr_)) == -1) {
    std::cout << "SAPListener() " << std::string(strerror(errno)) << " " << kIpaddr << ":" << kPort << "\n";
    exit(-1);
  }
}

SAPListener::~SAPListener() { close(sockfd_); }

std::unique_ptr<SAPListener> SAPListener::singleton_;
SAPListener &SAPListener::GetInstance() {
  if (!singleton_) singleton_ = std::make_unique<SAPListener>();
  return *singleton_;
}

void SAPListener::SAPListenerThread(SAPListener *sap) {
  struct timeval read_timeout;
  read_timeout.tv_sec = 0;
  read_timeout.tv_usec = 10;
  setsockopt(sap->sockfd_, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

  while (running_) {
    if (ssize_t bytes = recvfrom(sap->sockfd_, sap->udpdata.data(), kMaxUdpData, 0, nullptr, nullptr); bytes <= 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

      continue;
    }
    // Process SAP here
    sap->SapStore(sap->udpdata);
  }
}

void SAPListener::RegisterSapListener(std::string_view session_name, const SapCallback &callback) {
  callbacks_[std::string(session_name)] = callback;
}

void SAPListener::Start() {
  running_ = true;
  thread_ = std::thread(SAPListenerThread, this);
}

void SAPListener::Stop() {
  running_ = false;
  if (thread_.joinable()) thread_.join();
}

SdpTypeEnum SAPListener::GetType(const std::string_view &line) const {
  if (line.substr(0, 2).compare("v=") == 0) return SdpTypeEnum::kProtocolVersion;
  if (line.substr(0, 2).compare("o=") == 0) return SdpTypeEnum::kOriginatorSessionIdentifier;
  if (line.substr(0, 2).compare("s=") == 0) return SdpTypeEnum::kSessionName;
  if (line.substr(0, 2).compare("c=") == 0) return SdpTypeEnum::kConnectionInformation;
  if (line.substr(0, 2).compare("t=") == 0) return SdpTypeEnum::kTimeSessionActive;
  if (line.substr(0, 2).compare("m=") == 0) return SdpTypeEnum::kMediaNameAndTransportAddress;
  if (line.substr(0, 2).compare("a=") == 0) return SdpTypeEnum::kSessionAttribute;
  return SdpTypeEnum::kUnknown;
}

std::map<std::string, std::string, std::less<>> SAPListener::ParseAttributes(const std::string_view &line) const {
  std::map<std::string, std::string, std::less<>> attributes;
  std::string key;
  std::string value;
  bool attribute_name = true;

  // Step through characters
  for (char c : line) {
    if (c == ':') {
      attribute_name = false;
      continue;
    }
    if (attribute_name) {
      if (c != ' ') key += c;
    } else {
      // attribute_value
      if (c != ' ') value += c;
    }
  }
  attributes[key] = value;
  return attributes;
}

std::map<std::string, std::string, std::less<>> SAPListener::ParseAttributesEqual(const std::string &line) const {
  std::map<std::string, std::string, std::less<>> attributes;
  std::string key;
  std::string value;
  bool type = true;

  for (const char c : line) {
    if (c == '=') {
      type = false;
      continue;
    }
    if (c == ';') {
      type = true;
      attributes[key] = value;
      key = "";
      value = "";
      continue;
    }
    if (type) {
      if (c != ' ') key += c;
    } else {
      if (c != ' ') value += c;
    }
  }
  return attributes;
}

bool SAPListener::SapStore(std::array<uint8_t, kMaxUdpData> &rawdata) {
  std::map<std::string, std::string, std::less<>> attributes_map;

  const uint32_t *source = (uint32_t *)&udpdata[4];
  SDPMessage sdp;
  sdp.sdp_text = (char *)&rawdata[8];
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
        sdp.ip_address = line.substr(lastSpace + 1);
        size_t pos = sdp.ip_address.find('/');  // Find position of the first forward slash
        if (pos != std::string::npos) {         // If a forward slash is found
          sdp.ip_address =
              sdp.ip_address.substr(0, pos);  // Get substring from start to the position of the forward slash
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
      case SdpTypeEnum::kConnectionInformation:
        break;
      case SdpTypeEnum::kBandwidthInformation:
        break;
      case SdpTypeEnum::kSessionAttribute: {
        std::map<std::string, std::string, std::less<>> attributes_map_more = ParseAttributes(line);
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
  attributes_map_fmtp = ParseAttributesEqual(attributes_map["fmtp"]);
  attributes_map.insert(attributes_map_fmtp.begin(), attributes_map_fmtp.end());

  try {
    sdp.height = std::stoi(attributes_map["height"]);
    sdp.width = std::stoi(attributes_map["width"]);
    sdp.framerate = std::stoi(attributes_map["framerate"]);
    sdp.sampling = attributes_map["96sampling"];
  } catch (const std::invalid_argument &e [[maybe_unused]]) {
    return false;
  }

  announcements_[sdp.session_name] = sdp;

  // Check the callbacks

  for (const auto &[name, callback] : callbacks_) {
    if (name == sdp.session_name) {
      // We have a match, hit the callback
      callback(sdp);
    }
  }

  return true;
}

const std::map<std::string, SDPMessage, std::less<>> &SAPListener::GetSAPAnnouncements() const {
  return announcements_;
}

}  // namespace mediax::sap
