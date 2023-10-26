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

#include <array>
#include <chrono>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "rtp/rtp_internal_types.h"
#include "rtp/rtp_types.h"
#include "sap/sap_utils.h"

namespace mediax::sap {

bool SapAnnouncer::running_ = false;

SapAnnouncer::SapAnnouncer() {
  if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    DLOG(ERROR) << "socket creation failed";
    exit(EXIT_FAILURE);
  }

  memset(&multicast_addr_, 0, sizeof(multicast_addr_));
  multicast_addr_.sin_family = AF_INET;
  multicast_addr_.sin_addr.s_addr = inet_addr(mediax::rtp::kIpaddr);
  multicast_addr_.sin_port = htons(::mediax::rtp::kPort);

  // Select first found interface, can be overridden
  SetSourceInterface(0);
}

SapAnnouncer::~SapAnnouncer() {
  DeleteAllStreams();
  if (sockfd_ != -1) close(sockfd_);
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
  for (auto &stream_ : streams_) {
    stream_.deleted = false;
  }
  running_ = true;
  thread_ = std::thread(SapAnnouncementThread, this);
}
void SapAnnouncer::Stop() {
  // Only deletes the streams from the SAP/SDP announcement, they are still in the vector ready to be restarted
  DeleteAllStreams();
  running_ = false;
  if (thread_.joinable()) thread_.join();
}

void SapAnnouncer::DeleteAllStreams() {
  for (auto &stream_ : streams_) {
    if (stream_.deleted == false) {
      SendSapDeletion(stream_);
      stream_.deleted = true;
    }
  }
}
void SapAnnouncer::AddSapAnnouncement(const ::mediax::rtp::StreamInformation &stream_information) {
  streams_.push_back(stream_information);
}

void SapAnnouncer::DeleteAllSapAnnouncements() {
  // Delete all the live SAP announcements
  for (const auto &stream : streams_) {
    SendSapDeletion(stream);
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
void SapAnnouncer::SendSapPacket(const ::mediax::rtp::StreamInformation &stream_information, bool deletion) const {
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
    sampling = " profile-level-id=42A01E; packetisation-mode=0\r\n";
    mode = "H264";
  } else {
    id = 96;
    sampling = " sampling=" + ::mediax::sap::GetSdpColourspace(stream_information.encoding) +
               "; width=" + std::to_string(stream_information.width) +
               "; height=" + std::to_string(stream_information.height) + "; depth=" + depth + "; " + colorimetry +
               "progressive\r\n";
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
      std::to_string(id) + sampling + "a=framerate:" + std::to_string(stream_information.framerate) + "";

  if (stream_information.encoding == mediax::rtp::ColourspaceType::kColourspaceMono16) {
    sdp_msg +=
        "a=active-pixel-depth:16\r\n"
        "a=number-pixel-flags:2\r\n"
        "a=pixel-flags:saturated,ignored";
  }

  // Oversized 4k buffer for SAP/SDP
  std::array<uint8_t, 4069> buffer;
  if (deletion) {
    SapHeader header(0x24, 0, source_ipaddress_);
    memcpy(&buffer[0], &header, sizeof(header));
  } else {
    SapHeader header(0x20, 0, source_ipaddress_);
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

  ssize_t sent_bytes = sendto(sockfd_, buffer.data(), sizeof(SapHeader) + sdp_msg.size(), 0,
                              (const struct sockaddr *)(&multicast_addr_), sizeof(multicast_addr_));
  if (sent_bytes < 0) {
    perror("sendto failed");
    exit(EXIT_FAILURE);
  }
}

void SapAnnouncer::SapAnnouncementThread(SapAnnouncer *sap) {
  while (running_) {
    for (const auto &stream : sap->GetStreams()) {
      if (stream.deleted == false) sap->SendSapAnnouncement(stream);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void SapAnnouncer::SetSourceInterface(uint16_t select) { SetAddressHelper(select, false); }

void SapAnnouncer::ListInterfaces(uint16_t select) { SetAddressHelper(select, true); }

uint32_t SapAnnouncer::GetActiveStreamCount() const { return (uint32_t)streams_.size(); }

std::vector<::mediax::rtp::StreamInformation> &SapAnnouncer::GetStreams() { return streams_; }

void SapAnnouncer::SetAddressHelper(uint16_t select [[maybe_unused]], bool helper) {
#ifdef _WIN32
#pragma stream_information("TODO: Implement SetAddressHelper for Windows")
#else
  struct ifaddrs *ifaddr;

  if (getifaddrs(&ifaddr) == -1) {
    LOG(ERROR) << "Error getting local IP addresses";
    return;
  }

  for (struct ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == nullptr) {
      continue;
    }

    CheckAddresses(ifa, helper, select);
  }
  freeifaddrs(ifaddr);
#endif
}

void SapAnnouncer::CheckAddresses(struct ifaddrs *ifa, bool helper, uint16_t select) {
#ifdef _WIN32
#pragma stream_information("TODO: Implement CheckAddresses for Windows")
#else
  uint16_t count_interfaces = 0;
  std::array<char, INET_ADDRSTRLEN> addr_str;

  // Check for IPv4 address
  if (ifa->ifa_addr->sa_family == AF_INET) {
    auto sa = (struct sockaddr_in *)ifa->ifa_addr;
    inet_ntop(AF_INET, &(sa->sin_addr), addr_str.data(), INET_ADDRSTRLEN);

    // Exclude the loopback address
    if (strcmp(addr_str.data(), "127.0.0.1") != 0) {
      std::string postfix;
      if (helper) std::cout << "Interface: " << ifa->ifa_name << std::endl;
      // save the last one
      if (count_interfaces == select) {
        source_ipaddress_ = sa->sin_addr.s_addr;
        postfix = " <- selected";
      }
      if (helper) std::cout << "IPv4 Address: " << addr_str.data() << postfix << std::endl;
      count_interfaces++;
    }
  }
#endif
}

}  // namespace mediax::sap
