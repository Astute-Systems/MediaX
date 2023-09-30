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
/// \file sap_announcer_example.cc
///

#include <iostream>

#include "sap/sap_announcer.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  std::cout << argv[0] << " starting\n";

  mediax::sap::SAPAnnouncer &sap = mediax::sap::SAPAnnouncer::GetInstance();

  // Choose a source interface
  sap.ListInterfaces(0);

  sap.AddSAPAnnouncement(
      {"Stream 1", "192.168.1.1", 5000, 480, 640, 25, mediax::ColourspaceType::kColourspaceYuv, false});
  sap.AddSAPAnnouncement(
      {"Stream 2", "192.168.1.2", 6000, 800, 600, 30, mediax::ColourspaceType::kColourspaceYuv, false});
  sap.AddSAPAnnouncement(
      {"Stream 3", "192.168.1.3", 7000, 1920, 1024, 60, mediax::ColourspaceType::kColourspaceYuv, false});
  sap.AddSAPAnnouncement(
      {"Stream 4", "192.168.1.4", 8000, 1920, 1024, 60, mediax::ColourspaceType::kColourspaceRgb24, false});
  sap.AddSAPAnnouncement(
      {"Stream 5", "192.168.1.5", 9000, 1920, 1024, 60, mediax::ColourspaceType::kColourspaceMono8, false});
  sap.AddSAPAnnouncement(
      {"Stream 6", "192.168.1.6", 10000, 1920, 1024, 60, mediax::ColourspaceType::kColourspaceJpeg2000, false});
  sap.AddSAPAnnouncement(
      {"Stream 7", "192.168.1.7", 20000, 1920, 1024, 60, mediax::ColourspaceType::kColourspaceH264Part10, false});

  // Start all the streams for 10 seconds
  sap.Start();
  sleep(10);
  sap.Stop();

  // Thread is running but all streams stopped at this point for 2 seconds
  sleep(2);

  // Re-start all the streams for 5 seconds
  sap.Start();
  sleep(5);
  sap.Stop();

  return 0;
}