//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//'
/// \brief A SAP/SDP announcement example
///
/// \file sap-announcer.cc
///

#include <unistd.h>

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "sap/sap_announcer.h"

// Signal handler
volatile sig_atomic_t stop_signal = 0;

// Signal handler
void signal_handler(int signal) {
  std::cout << "Signal " << signal << " caught, exiting..." << std::endl;
  stop_signal = 1;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  std::cout << argv[0] << " starting\n";

  // [Sap example announcer]
  // Get the SAP/SDP announcment singleton instance
  mediax::sap::SapAnnouncer &sap = mediax::sap::SapAnnouncer::GetInstance();
  // Set the source IP address
  sap.SetSourceInterface();

  // Add all your stream announcments here
  sap.AddSapAnnouncement(
      {"Pi Camera 1", "239.192.1.1", 5004, 720, 1280, 25, mediax::rtp::ColourspaceType::kColourspaceH264Part10, false});
  sap.AddSapAnnouncement(
      {"Pi Camera 2", "239.192.2.1", 5004, 720, 1280, 25, mediax::rtp::ColourspaceType::kColourspaceH264Part10, false});
  sap.AddSapAnnouncement(
      {"Pi Camera 3", "239.192.3.1", 5004, 720, 1280, 25, mediax::rtp::ColourspaceType::kColourspaceH264Part10, false});
  sap.AddSapAnnouncement(
      {"Pi Camera 4", "239.192.4.1", 5004, 720, 1280, 25, mediax::rtp::ColourspaceType::kColourspaceH264Part10, false});
  sap.AddSapAnnouncement(
      {"Pi Camera 5", "239.192.5.1", 5004, 720, 1280, 25, mediax::rtp::ColourspaceType::kColourspaceH264Part10, false});
  sap.AddSapAnnouncement(
      {"Pi Camera 6", "239.192.6.1", 5004, 720, 1280, 25, mediax::rtp::ColourspaceType::kColourspaceH264Part10, false});
  // Start all the streams
  sap.Start();

  // Loop till SIGINT
  while (!stop_signal) {
    sleep(1);
  }

  sap.Stop();

  return 0;
}
