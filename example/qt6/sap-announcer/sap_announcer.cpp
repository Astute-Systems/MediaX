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

#include <glog/logging.h>

#include <QCoreApplication>
#include <QSap>

#include "rtp/rtp.h"
#include "sap_receive.h"

int main(int argc, char* argv[]) {
  QCoreApplication a(argc, argv);
  // Initalise the RTP library
  google::InitGoogleLogging(argv[0]);
  mediax::InitRtp(argc, argv);
  // Create a QtTransmit object

  mediax::qt6::QtSapListener listener;
  listener.start();

  // Sleep for 2 seconds
  std::cout << "Waiting SAP/SDP announcements\n";

  QtSapReceiver receiver;

  // Recieve the announcments
  QObject::connect(&listener, &mediax::qt6::QtSapListener::sapData, &receiver, &QtSapReceiver::newSap);

  int ret = a.exec();
  listener.stop();
  return ret;
}
