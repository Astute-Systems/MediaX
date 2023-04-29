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

#include <iostream>

#include "sap_listener.h"

int main() {
  sap::SAPListener& sap = sap::SAPListener::GetInstance();

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

  const std::map<std::string, sap::SDPMessage>& announcements = sap.GetSAPAnnouncements();

  std::cout << ">>>>>>>>> List of SAP announcements :\n";
  for (const auto& announcement : announcements) {
    sap::SDPMessage sdp = announcement.second;
    std::cout << "SDP> name: " << sdp.session_name << ", source: " << sdp.ip_address_source
              << ", ipaddr: " << sdp.ip_address << ":" << sdp.port << ", height: " << sdp.height
              << ", width: " << sdp.width << ", framerate: " << sdp.framerate << ", sampling: " << sdp.sampling << "\n";
  }

  return 0;
}