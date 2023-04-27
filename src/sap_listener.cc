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
/// \file sap_listener.cc

#include "sap_listener.h"

#include <chrono>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace sap {

bool SAPListener::running_ = false;

SAPListener::SAPListener() {
  if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }  // namespace sap

  memset(&multicast_addr_, 0, sizeof(multicast_addr_));
  multicast_addr_.sin_family = AF_INET;
  multicast_addr_.sin_addr.s_addr = inet_addr(kIpaddr.c_str());
  multicast_addr_.sin_port = htons(kPort);

  // bind socket to port
  if (bind(sockfd_, (struct sockaddr *)&multicast_addr_, sizeof(multicast_addr_)) == -1) {
    std::cout << "ERROR binding socket\n";
    exit(-1);
  }
}

SAPListener::~SAPListener() { close(sockfd_); }

SAPListener &SAPListener::GetInstance() {
  static SAPListener singleton_;
  return singleton_;
}

void SAPListener::SAPListenerThread(SAPListener *sap) {
  while (running_) {
    if (ssize_t bytes = recvfrom(sap->sockfd_, sap->udpdata.data(), kMaxUdpData, 0, nullptr, nullptr); bytes <= 0) {
      std::cout << "Packet Received\n";
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void SAPListener::Start() {
  running_ = true;
  thread_ = std::thread(SAPListenerThread, this);
}

void SAPListener::Stop() {
  running_ = false;
  thread_.join();
}

const std::vector<SDPMessage> &SAPListener::GetSAPAnnouncements() const { return announcements_; }

}  // namespace sap