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
  sap::SAPAnnouncer sap;

  // Choose a source interface
  sap.ListAddresses();

  sap.AddSAPAnnouncement({"Stream 1", "192.168.1.1", 5000, 480, 640, 25});
  sap.AddSAPAnnouncement({"Stream  2", "192.168.1.2", 6000, 480, 640, 25});
  sap.AddSAPAnnouncement({"Stream 3", "192.168.1.3", 7000, 480, 640, 25});

  sap.BroadcastSAPAnnouncements();

  return 0;
}