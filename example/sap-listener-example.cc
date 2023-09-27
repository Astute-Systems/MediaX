//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \file sap-listener.cc
///

#include <iostream>

#include "sap/sap_listener.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  std::cout << argv[0] << " starting\n";

  mediax::sap::SAPListener& sap = mediax::sap::SAPListener::GetInstance();

  std::cout << "Waiting for 2 seconds for all SAP/SDP announcements\n";

  // Listen for 2 seconds and dump out the SAP announcements seen
  sap.Start();
  sleep(2);
  sap.Stop();

  const std::map<std::string, mediax::sap::SDPMessage, std::less<>>& announcements = sap.GetSAPAnnouncements();

  if (announcements.empty()) {
    std::cout << "No SAP/SDP announcements seen\n";
    return 0;
  }

  for (const auto& [name, sdp] : announcements) {
    std::cout << "SDP> name: " << sdp.session_name << ", source: " << sdp.ip_address_source
              << ", ipaddr: " << sdp.ip_address << ":" << sdp.port << ", height: " << sdp.height
              << ", width: " << sdp.width << ", framerate: " << sdp.framerate << ", sampling: " << sdp.sampling << "\n";
  }

  return 0;
}