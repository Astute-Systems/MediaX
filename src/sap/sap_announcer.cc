//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief Session Announcement Protocol (SDP) implementation for announcement of the stream data. The SAP packet
/// contains the Session Description Protocol (SDP).
///
/// \file sap_announcer.cc
///

#include "sap/sap_announcer.h"

#include <glog/logging.h>
#include <ifaddrs.h>
#include <net/if.h>

#include <array>
#include <chrono>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "rtp/rtp_internal_types.h"
#include "rtp/rtp_types.h"
#include "sap/sap_utils.h"

namespace mediax::sap {

SapAnnouncer::SapAnnouncer() {
  memset(&multicast_addr_, 0, sizeof(multicast_addr_));
  multicast_addr_.sin_family = AF_INET;
  multicast_addr_.sin_addr.s_addr = inet_addr(mediax::rtp::kIpaddr);
  multicast_addr_.sin_port = htons(::mediax::rtp::kSapPort);
}

SapAnnouncer::~SapAnnouncer() {
  DeleteAllStreams();
  Stop();
}

SapAnnouncer &SapAnnouncer::GetInstance() {
  static SapAnnouncer singleton_;
  return singleton_;
}

void SapAnnouncer::Start() {
  if (running_) {
    DLOG(WARNING) << "SapAnnouncer already running, called twice?\n";
    return;
  }

  thread_ = std::thread(SapAnnouncementThread, this);

  // Wait for thread to start
  while (running_ == false) {
    // Wait 1ms;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void SapAnnouncer::Stop() {
  // Only deletes the streams from the SAP/SDP announcement, they are still in the vector ready to be restarted
  DeleteAllStreams();
  running_ = false;
  if (thread_.joinable()) thread_.join();
}

void SapAnnouncer::Restart() {
  // Restart any deleted SAP/SDP announcements
  for (const auto &stream : streams_) {
    if (stream.deleted) {
      UndeleteSapAnnouncement(stream.session_name);
    }
  }
}

bool SapAnnouncer::Active() const { return running_; }

void SapAnnouncer::DeleteAllStreams() const {
  for (const auto &stream : streams_) {
    if (stream.deleted == false) {
      SendSapDeletion(stream);
    }
  }
}

void SapAnnouncer::AddSapAnnouncement(const ::mediax::rtp::StreamInformation &stream_information) {
  // Ensure session_name is unique
  for (auto &stream : streams_) {
    if (stream.session_name == stream_information.session_name) {
      stream = stream_information;
      return;
    }
  }
  streams_.push_back(stream_information);
}

::mediax::rtp::StreamInformation &SapAnnouncer::GetSapAnnouncment(std::string session_name) {
  for (auto &stream_ : streams_) {
    if (stream_.session_name == session_name) {
      return stream_;
    }
  }
  // Return the first stream if not found
  return streams_[0];
}

void SapAnnouncer::DeleteSapAnnouncement(std::string_view session_name) {
  for (auto &stream_ : streams_) {
    if (stream_.session_name == session_name) {
      SendSapDeletion(stream_);
      stream_.deleted = true;
    }
  }
}

void SapAnnouncer::UndeleteSapAnnouncement(std::string_view session_name) {
  for (auto &stream_ : streams_) {
    if (stream_.session_name == session_name) {
      stream_.deleted = false;
    }
  }
}

void SapAnnouncer::DeleteAllSapAnnouncements() {
  // Delete all the live SAP announcements
  for (auto &stream : streams_) {
    if (running_) SendSapDeletion(stream);
    stream.deleted = true;
    streams_.clear();
  }
}

void SapAnnouncer::SendSapAnnouncement(const ::mediax::rtp::StreamInformation &stream_information) const {
  SendSapPacket(stream_information, false);
}

void SapAnnouncer::SendSapDeletion(const ::mediax::rtp::StreamInformation &stream_information) const {
  SendSapPacket(stream_information, true);
}

// Function to send a SAP announcement
int SapAnnouncer::SendSapPacket(const ::mediax::rtp::StreamInformation &stream_information, bool deletion) const {
  if (sockfd_ == -1) {
    std::cerr << "SAP socket not open, did you call Start()\n";
    return 1;
  }
  std::string depth;
  std::string colorimetry;
  std::string mode = "raw";

  switch (stream_information.encoding) {
    case mediax::rtp::ColourspaceType::kColourspaceMono8:
      depth = "8";
      break;
    case mediax::rtp::ColourspaceType::kColourspaceMono16:
      depth = "16";
      break;
    case mediax::rtp::ColourspaceType::kColourspaceRgb24:
      depth = "8";
      colorimetry = "colorimetry=BT709-2; ";
    case mediax::rtp::ColourspaceType::kColourspaceYuv:
      depth = "8";
      colorimetry = "colorimetry=BT601-5; ";
    default:
      break;
  }

  std::string sampling = "";
  uint32_t id;
  if ((stream_information.encoding == mediax::rtp::ColourspaceType::kColourspaceNv12) ||
      (stream_information.encoding == mediax::rtp::ColourspaceType::kColourspaceH264Part10)) {
    id = 103;
    sampling = " profile-level-id=42A01E; packetisation-mode=0";
    mode = "H264";
  } else if (stream_information.encoding == mediax::rtp::ColourspaceType::kColourspaceJpeg2000) {
    id = 100;
    sampling = " sampling=YCbCr-4:2:2; width=" + std::to_string(stream_information.width) +
               "; height=" + std::to_string(stream_information.height);
    mode = "jpeg2000";
  } else {
    id = 96;
    sampling = " sampling=" + ::mediax::sap::GetSdpColourspace(stream_information.encoding) +
               "; width=" + std::to_string(stream_information.width) +
               "; height=" + std::to_string(stream_information.height) + "; depth=" + depth + "; " + colorimetry +
               "progressive";
  }
  // Convert source_ipaddress_ to std::string
  std::string addr_str;
  addr_str.resize(INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &source_ipaddress_, addr_str.data(), INET_ADDRSTRLEN);
  // Trim to size of string
  addr_str.resize(strlen(addr_str.c_str()));

  // Prepare SDP stream_information
  std::string sdp_msg =
      "v=0\r\n"
      "o=- 3394362021 3394362021 IN IP4 " +
      addr_str + "\r\n" + "s=" + stream_information.session_name +
      "\r\n"
      "c=IN IP4 " +
      stream_information.hostname +
      "/15\r\n"
      "t=0 0\r\n"
      "m=video " +
      std::to_string(stream_information.port) + " RTP/AVP " + std::to_string(id) +
      "\r\n"
      "a=rtpmap:" +
      std::to_string(id) + " " + mode +
      "/90000\r\n"
      "a=fmtp:" +
      std::to_string(id) + sampling + "\r\n" + "a=framerate:" + std::to_string(stream_information.framerate) + "";

  if (stream_information.encoding == mediax::rtp::ColourspaceType::kColourspaceMono16) {
    sdp_msg +=
        "a=active-pixel-depth:16\r\n"
        "a=number-pixel-flags:2\r\n"
        "a=pixel-flags:saturated,ignored";
  }

  // Oversized 4k buffer for SAP/SDP
  std::array<uint8_t, 4069> buffer;
  if (deletion) {
    SapHeader header(0x24, 0, 0, source_ipaddress_);
    memcpy(&buffer[0], &header, sizeof(header));
  } else {
    SapHeader header(0x20, 0, 0, source_ipaddress_);
    memcpy(&buffer[0], &header, sizeof(header));
  }

  // Copy the payload type into the buffer (optional but we send anyway)
  std::string payload_type = "application/sdp";
  // Add null terminator
  payload_type.push_back('\0');

  memcpy(&buffer[sizeof(SapHeader)], payload_type.data(), payload_type.size());
  // Copy the SDP message into the buffer
  memcpy(&buffer[sizeof(SapHeader) + payload_type.size()], sdp_msg.data(), sdp_msg.size());

  // Calculate the 16 bit hash dor the SAP message
  uint16_t hash = 0;
  for (size_t i = 0; i < sdp_msg.size(); i++) {
    hash += sdp_msg[i];
  }
  // Copy the hash into the buffer
  memcpy(&buffer[2], &hash, sizeof(hash));

  ssize_t sent_bytes = sendto(sockfd_, buffer.data(), sizeof(SapHeader) + payload_type.size() + sdp_msg.size(), 0,
                              (const struct sockaddr *)(&multicast_addr_), sizeof(multicast_addr_));
  if (sent_bytes < 0) {
    perror("sendto failed");
    exit(EXIT_FAILURE);
  }
  return 0;
}

void SapAnnouncer::SapAnnouncementThread(SapAnnouncer *sap) {
  sap->sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);

  // Enable multicast loopback
  if (int loop = 1; setsockopt(sap->sockfd_, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0) {
    perror("setsockopt");
  }
  sap->running_ = true;
  while (sap->running_) {
    for (const auto &stream : sap->GetStreams()) {
      if (!stream.deleted) sap->SendSapAnnouncement(stream);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  if (sap->sockfd_ != -1) close(sap->sockfd_);
  sap->sockfd_ = -1;
}

void SapAnnouncer::SetSourceInterface(uint32_t select) {
  std::map<uint32_t, std::string> interfaces;
  interfaces = GetInterfaces();
  if (interfaces.empty()) {
    DLOG(ERROR) << "No interfaces found";
    return;
  }
  if (select >= interfaces.size()) {
    DLOG(ERROR) << "Interface select out of range";
    return;
  }
  std::string interface_name = interfaces[select];
  source_ipaddress_ = GetIpv4Address(interfaces[select]);
}

uint32_t SapAnnouncer::GetIpv4Address(std::string interface_name) const {
  // Get the address tof the interface
  struct ifaddrs *ifaddr, *ifa;
  if (getifaddrs(&ifaddr) == -1) {
    perror("getifaddrs failed");
    return 0;
  }

  for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == nullptr) {
      continue;
    }

    if (ifa->ifa_addr->sa_family == AF_INET && (ifa->ifa_flags & IFF_UP) && (ifa->ifa_flags & IFF_RUNNING)) {
      struct sockaddr_in *s4 = reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr);
      std::string ifname = ifa->ifa_name;
      if (ifname == interface_name) {
        freeifaddrs(ifaddr);
        return s4->sin_addr.s_addr;
      }
    }
  }

  return 0;
}

std::string SapAnnouncer::GetIpv4AddressString(std::string interface_name) {
  const uint32_t address = GetIpv4Address(interface_name);
  std::string addr_str;
  addr_str.resize(INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &address, addr_str.data(), INET_ADDRSTRLEN);
  // Trim to size of string
  addr_str.resize(strlen(addr_str.c_str()));
  return addr_str;
}

std::map<uint32_t, std::string> SapAnnouncer::GetInterfaces() {
  std::map<uint32_t, std::string> interfaces;
  uint32_t count = 0;

  struct ifaddrs *ifaddr, *ifa;
  if (getifaddrs(&ifaddr) == -1) {
    perror("getifaddrs failed");
    return interfaces;
  }

  for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == nullptr) {
      continue;
    }

    if (ifa->ifa_addr->sa_family == AF_PACKET && (ifa->ifa_flags & IFF_UP) && (ifa->ifa_flags & IFF_RUNNING)) {
      struct sockaddr_ll *sll = reinterpret_cast<struct sockaddr_ll *>(ifa->ifa_addr);
      std::string ifname = ifa->ifa_name;
      // Dont store loopback
      if (ifname != "lo") {
        interfaces[count] = ifname;
        count++;
      }
    }
  }

  freeifaddrs(ifaddr);
  return interfaces;
}

uint32_t SapAnnouncer::GetActiveStreamCount() const { return (uint32_t)streams_.size(); }

std::vector<::mediax::rtp::StreamInformation> &SapAnnouncer::GetStreams() { return streams_; }

}  // namespace mediax::sap
