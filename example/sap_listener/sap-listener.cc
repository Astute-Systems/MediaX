//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief A SAP/SDP listener example
///
/// \file sap-example.cc
///

#include <iostream>

#include "sap/sap_listener.h"

// Callback to get SAP announcments
void Callback(const mediax::sap::SdpMessage* message, void* data) {
  std::cout << "SDP> name: " << message->session_name << ", source: " << message->ip_address_source
            << ", ipaddr: " << message->ip_address << ":" << message->port << ", height: " << message->height
            << ", width: " << message->width << ", framerate: " << message->framerate
            << ", sampling: " << message->sampling << "\n";
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  std::cout << argv[0] << " starting\n";

  mediax::sap::SapListener& sap = mediax::sap::SapListener::GetInstance();

  std::cout << "Waiting for 2 seconds for all SAP/SDP announcements\n";

  // Hook up the callback
  sap.RegisterSapListener("", Callback, nullptr);

  // Listen for 2 seconds and dump out the SAP announcements seen
  sap.Start();
  sleep(2);
  sap.Stop();

  const std::map<std::string, mediax::sap::SdpMessage, std::less<>>& announcements = sap.GetSapAnnouncements();

  if (announcements.empty()) {
    std::cout << "No SAP/SDP announcements seen\n";
    return 0;
  }

  return 0;
}