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
/// \file sap-example.cc
///

#include "sap_announcer.h"

int main() {
  sap::SAPAnnouncer &sap = sap::SAPAnnouncer::GetInstance();

  // Choose a source interface
  sap.ListInterfaces(0);

  sap.AddSAPAnnouncement({"Stream 1", "192.168.1.1", 5000, 480, 640, 25, false});
  sap.AddSAPAnnouncement({"Stream 2", "192.168.1.2", 6000, 800, 600, 30, false});
  sap.AddSAPAnnouncement({"Stream 3", "192.168.1.3", 7000, 1920, 1024, 60, false});

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