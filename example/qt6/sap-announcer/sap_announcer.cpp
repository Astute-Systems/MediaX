//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \file sap_announcer.cpp
///

#include <QCoreApplication>
#include <QSap>

#include "rtp/rtp.h"

int main(int argc, char* argv[]) {
  QCoreApplication a(argc, argv);
  // Initalise the RTP library
  mediax::InitRtp(argc, argv);
  // Create a QtSapAnnouncer object
  mediax::qt6::QtSapAnnouncer announcer;
  // Add a single sap announcments
  ::mediax::rtp::StreamInformation stream_information = {
      .session_name = "Test SAP announcment",
      .hostname = "127.0.0.1",
      .port = 5004,
      .height = 1080,
      .width = 1920,
      .framerate = 25,
      .encoding = mediax::rtp::ColourspaceType::kColourspaceH264Part10};
  announcer.addSapAnnouncement(stream_information);
  announcer.start();
  std::cout << "Sending SAP/SDP announcements\n";
  int ret = a.exec();
  announcer.stop();
  return ret;
}
