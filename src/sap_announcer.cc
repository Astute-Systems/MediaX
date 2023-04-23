//
// Copyright (C) 2023 DefenceX PTY LTD. All rights reserved.
//
// This software is distributed under the included copyright license.
// Any redistribution or reproduction, in part or in whole, in any form or medium, is strictly prohibited without the
// prior written consent of DefenceX PTY LTD.
//
// For any inquiries or concerns, please contact:
// DefenceX PTY LTD
// Email: enquiries@defencex.ai
//
/// \file sap_announcer.cc

#include "sap_announcer.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace sap {

void SAPAnnouncer::AddSAPAnnouncement(const SAPMessage &message) { streams_.push_back(message); }

// Function to send a SAP announcement
void SAPAnnouncer::SendSAPAnnouncement(const SAPMessage &message) {
  // Prepare SDP message
  std::string sdp_msg =
      "v=0\r\n"
      "o=- 3394362021 3394362021 IN IP4 192.168.204.5\r\n"
      "s=" +
      message.sessionName +
      "\r\n"
      "c=IN IP4 " +
      kIpaddr +
      "/15\r\n"
      "t=0 0\r\n"
      "m=video " +
      std::to_string(kPort) +
      " RTP/AVP 96\r\n"
      "a=rtpmap:96 raw/90000\r\n"
      "a=fmtp:96 sampling=YCbCr-4:2:2; width=" +
      std::to_string(message.width) + "; height=" + std::to_string(message.height) +
      "; depth=8; colorimetry=BT601-5; progressive\r\n"
      "a=framerate:" +
      std::to_string(message.framerate) + "\r\n";

  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in multicast_addr;
  memset(&multicast_addr, 0, sizeof(multicast_addr));
  multicast_addr.sin_family = AF_INET;
  multicast_addr.sin_addr.s_addr = inet_addr(kIpaddr.c_str());
  multicast_addr.sin_port = htons(kPort);

  SAPHeader header(0x20, 0, source_ipaddress_);

  // Oversized 4k buffer for SAP/SDP
  std::array<uint8_t, 4069> buffer;
  memcpy(&buffer[0], &header, sizeof(header));
  memcpy(&buffer[sizeof(header)], sdp_msg.data(), sdp_msg.size());

  ssize_t sent_bytes = sendto(sockfd, buffer.data(), sizeof(header) + sdp_msg.size(), 0,
                              reinterpret_cast<const struct sockaddr *>(&multicast_addr), sizeof(multicast_addr));
  if (sent_bytes < 0) {
    perror("sendto failed");
    exit(EXIT_FAILURE);
  }

  close(sockfd);
}

// Function to broadcast SAP announcements for a list of streams
void SAPAnnouncer::BroadcastSAPAnnouncements() {
  while (true) {
    for (const auto &stream_ : streams_) {
      SendSAPAnnouncement(stream_);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void SAPAnnouncer::SetSourceInterface(uint16_t select) { SetAddressHelper(select, false); }

void SAPAnnouncer::ListInterfaces(uint16_t select) { SetAddressHelper(select, true); }

void SAPAnnouncer::SetAddressHelper(uint16_t select, bool helper) {
  uint16_t count_interfaces = 0;
  struct ifaddrs *ifaddr;
  char addr_str[INET_ADDRSTRLEN];

  if (getifaddrs(&ifaddr) == -1) {
    std::cerr << "Error getting local IP addresses" << std::endl;
    return;
  }

  for (struct ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == nullptr) {
      continue;
    }

    // Check for IPv4 address
    if (ifa->ifa_addr->sa_family == AF_INET) {
      auto sa = (struct sockaddr_in *)ifa->ifa_addr;
      inet_ntop(AF_INET, &(sa->sin_addr), addr_str, INET_ADDRSTRLEN);

      // Exclude the loopback address
      if (strcmp(addr_str, "127.0.0.1") != 0) {
        std::string postfix;
        if (helper) std::cout << "Interface: " << ifa->ifa_name << std::endl;
        // save the last one
        if (count_interfaces == select) {
          source_ipaddress_ = sa->sin_addr.s_addr;
          postfix = " <- selected";
        }
        if (helper) std::cout << "IPv4 Address: " << addr_str << postfix << std::endl;
        count_interfaces++;
      }
    }
  }
  freeifaddrs(ifaddr);
}

}  // namespace sap
