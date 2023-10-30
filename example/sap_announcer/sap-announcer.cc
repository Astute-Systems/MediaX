//
// Copyright (c) 2023, DefenceX PTY LTD
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

  // Add all your stream announcments here
  sap.AddSapAnnouncement(
      {"Stream 1", "239.192.1.1", 5000, 480, 640, 25, mediax::rtp::ColourspaceType::kColourspaceYuv, false});
  sap.AddSapAnnouncement(
      {"Stream 2", "239.192.1.2", 6000, 800, 600, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false});
  sap.AddSapAnnouncement(
      {"Stream 3", "239.192.1.3", 7000, 1920, 1024, 60, mediax::rtp::ColourspaceType::kColourspaceYuv, false});
  sap.AddSapAnnouncement(
      {"Stream 4", "239.192.1.4", 8000, 1920, 1024, 60, mediax::rtp::ColourspaceType::kColourspaceRgb24, false});
  sap.AddSapAnnouncement(
      {"Stream 5", "239.192.1.5", 9000, 1920, 1024, 60, mediax::rtp::ColourspaceType::kColourspaceMono8, false});
  sap.AddSapAnnouncement(
      {"Stream 6", "239.192.1.6", 10000, 1920, 1024, 60, mediax::rtp::ColourspaceType::kColourspaceJpeg2000, false});
  sap.AddSapAnnouncement(
      {"Stream 7", "239.192.1.7", 20000, 1920, 1024, 60, mediax::rtp::ColourspaceType::kColourspaceH264Part10, false});
  // Start all the streams
  sap.Start();
  // [Sap example announcer]

  // Wait 10 seconds
  sleep(10);
  sap.Stop();

  // Thread is running but all streams stopped at this point for 2 seconds
  sleep(2);

  // Re-start all the streams for 5 seconds
  sap.Start();
  sleep(5);
  // [Sap example stop]

  sap.DeleteAllSapAnnouncements();
  // Kill the SAP announcer, can be re-started by calling Start(); again
  sap.Stop();
  // [Sap example stop]

  sap.Start();
  sap.AddSapAnnouncement(
      {"Stream 10", "239.192.1.250", 5004, 480, 640, 25, mediax::rtp::ColourspaceType::kColourspaceH264Part10, false});
  sap.AddSapAnnouncement(
      {"Stream 11", "239.192.1.252", 5004, 800, 600, 30, mediax::rtp::ColourspaceType::kColourspaceH264Part10, false});
  sap.AddSapAnnouncement({"Stream 12", "239.192.3.252", 5004, 1920, 1024, 60,
                          mediax::rtp::ColourspaceType::kColourspaceH264Part10, false});

  sleep(5);

  sap.DeleteSapAnnouncement("Stream 11");

  // Loop till SIGINT
  while (!stop_signal) {
    sleep(1);
  }

  // Should be two live stream now, run till CTL+C

  return 0;
}
