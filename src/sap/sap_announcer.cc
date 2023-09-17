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

#include <array>
#include <chrono>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "rtp/rtp_types.h"
#include "sap/sap_utils.h"

namespace mediax::sap {

bool SAPAnnouncer::running_ = false;

SAPAnnouncer::SAPAnnouncer() {
  if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&multicast_addr_, 0, sizeof(multicast_addr_));
  multicast_addr_.sin_family = AF_INET;
  multicast_addr_.sin_addr.s_addr = inet_addr(kIpaddr);
  multicast_addr_.sin_port = htons(kPort);

  // Select first found interface, can be overridden
  SetSourceInterface(0);
}

SAPAnnouncer::~SAPAnnouncer() {
  DeleteAllStreams();
  close(sockfd_);
}

SAPAnnouncer &SAPAnnouncer::GetInstance() {
  static SAPAnnouncer singleton_;
  return singleton_;
}

void SAPAnnouncer::Start() {
  if (running_) return;
  for (auto &stream_ : streams_) {
    stream_.deleted = false;
  }
  running_ = true;
  thread_ = std::thread(SAPAnnouncementThread, this);
}
void SAPAnnouncer::Stop() {
  // Only deletes the streams from the SAP/SDP announcement, they are still in the vector ready to be restarted
  DeleteAllStreams();
  running_ = false;
  thread_.join();
}

void SAPAnnouncer::DeleteAllStreams() {
  for (auto &stream_ : streams_) {
    if (stream_.deleted == false) {
      SendSAPDeletion(stream_);
      stream_.deleted = true;
    }
  }
}
void SAPAnnouncer::AddSAPAnnouncement(const SAPMessage &message) { streams_.push_back(message); }

void SAPAnnouncer::DeleteAllSAPAnnouncements() { streams_.clear(); }

void SAPAnnouncer::SendSAPAnnouncement(const SAPMessage &message) const { SendSAPPacket(message, false); }

void SAPAnnouncer::SendSAPDeletion(const SAPMessage &message) const { SendSAPPacket(message, true); }

// Function to send a SAP announcement
void SAPAnnouncer::SendSAPPacket(const SAPMessage &message, bool deletion) const {
  std::string depth;
  std::string colorimetry;

  switch (message.colourspace) {
    case mediax::ColourspaceType::kColourspaceMono8:
      depth = "8";
      break;
    case mediax::ColourspaceType::kColourspaceMono16:
      depth = "16";
      break;
    case mediax::ColourspaceType::kColourspaceRgb24:
      depth = "8";
      colorimetry = "colorimetry=BT709-2; ";
    case mediax::ColourspaceType::kColourspaceYuv:
      depth = "8";
      colorimetry = "colorimetry=BT601-5; ";
    default:
      break;
  }

  // Prepare SDP message
  std::string sdp_msg =
      "v=0\r\n"
      "o=- 3394362021 3394362021 IN IP4 " +
      message.ipAddress +
      "\r\n"
      "s=" +
      message.sessionName +
      "\r\n"
      "c=IN IP4 " +
      kIpaddr +
      "/15\r\n"
      "t=0 0\r\n"
      "m=video " +
      std::to_string(message.port) +
      " RTP/AVP 96\r\n"
      "a=rtpmap:96 raw/90000\r\n"
      "a=fmtp:96 sampling=" +
      GetSdpColourspace(message.colourspace) + "; width=" + std::to_string(message.width) +
      "; height=" + std::to_string(message.height) + "; depth=" + depth + "; " + colorimetry +
      "progressive\r\n"
      "a=framerate:" +
      std::to_string(message.framerate) + "";

  if (message.colourspace == mediax::ColourspaceType::kColourspaceMono16) {
    sdp_msg +=
        "a=active-pixel-depth:16\r\n"
        "a=number-pixel-flags:2\r\n"
        "a=pixel-flags:saturated,ignored";
  }

  // Oversized 4k buffer for SAP/SDP
  std::array<uint8_t, 4069> buffer;
  if (deletion) {
    SAPHeader header(0x24, 0, source_ipaddress_);
    memcpy(&buffer[0], &header, sizeof(header));
  } else {
    SAPHeader header(0x20, 0, source_ipaddress_);
    memcpy(&buffer[0], &header, sizeof(header));
  }

  memcpy(&buffer[sizeof(SAPHeader)], sdp_msg.data(), sdp_msg.size());

  ssize_t sent_bytes = sendto(sockfd_, buffer.data(), sizeof(SAPHeader) + sdp_msg.size(), 0,
                              (const struct sockaddr *)(&multicast_addr_), sizeof(multicast_addr_));
  if (sent_bytes < 0) {
    perror("sendto failed");
    exit(EXIT_FAILURE);
  }
}

void SAPAnnouncer::SAPAnnouncementThread(SAPAnnouncer *sap) {
  while (running_) {
    for (const auto &stream_ : sap->GetStreams()) {
      sap->SendSAPAnnouncement(stream_);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void SAPAnnouncer::SetSourceInterface(uint16_t select) { SetAddressHelper(select, false); }

void SAPAnnouncer::ListInterfaces(uint16_t select) { SetAddressHelper(select, true); }

void SAPAnnouncer::SetAddressHelper(uint16_t select [[maybe_unused]], bool helper) {
#ifdef _WIN32
#pragma message("TODO: Implement SetAddressHelper for Windows")
#else
  struct ifaddrs *ifaddr;

  if (getifaddrs(&ifaddr) == -1) {
    std::cerr << "Error getting local IP addresses" << std::endl;
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

void SAPAnnouncer::CheckAddresses(struct ifaddrs *ifa, bool helper, uint16_t select) {
#ifdef _WIN32
#pragma message("TODO: Implement CheckAddresses for Windows")
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
