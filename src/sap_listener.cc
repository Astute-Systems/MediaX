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

#include <fcntl.h>  // for fcntl(), F_GETFL, F_SETFL, O_NONBLOCK

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
  if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }  // namespace sap

  // Set the socket to non-blocking mode
  int flags = fcntl(sockfd_, F_GETFL, 0);
  if (flags == -1) {
    perror("fcntl F_GETFL failed");
    exit(EXIT_FAILURE);
  }

  if (fcntl(sockfd_, F_SETFL, flags | O_NONBLOCK) == -1) {
    perror("fcntl F_SETFL O_NONBLOCK failed");
    exit(EXIT_FAILURE);
  }

  memset(&multicast_addr_, 0, sizeof(multicast_addr_));
  multicast_addr_.sin_family = AF_INET;
  multicast_addr_.sin_port = htons(kPort);
  multicast_addr_.sin_addr.s_addr = htonl(INADDR_ANY);

  // join the multicast group
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(kIpaddr.c_str());
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(sockfd_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    perror("setsockopt IP_ADD_MEMBERSHIP");
    exit(EXIT_FAILURE);
  }

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
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }
    // Process SAP here
    std::cout << "+++++++++++++++++++++++SAP/SDP+++++++++++++++++++++++++\n";
    std::cout << (char *)&sap->udpdata[8] << "\n";
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