//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \file sap_receive.h
///

#include "sap_receive.h"

#include <iostream>
// For std::put_time
#include <ctime>
#include <iomanip>

void QtSapReceiver::newSap(const QString name, const ::mediax::sap::SdpMessage message) {
  // Print the time
  std::time_t t = std::time(nullptr);
  // Concise time
  std::cout << std::put_time(std::localtime(&t), "%F %T") << ":";
  // Print the message
  std::cout << message.session_name << ":";
  std::cout << message.ip_address_source << ":";
  std::cout << message.ip_address << ":";
  std::cout << message.port << ":";
  std::cout << message.framerate << ":";

  // Replace \n with \n\t
  std::string sdp_text = message.sdp_text;
  std::string::size_type pos = 0;
  // Must initialize
  while ((pos = sdp_text.find("\n", pos)) != std::string::npos) {
    sdp_text.replace(pos, 1, "\n\t");
    pos += 2;
  }
  std::cout << "\n\t" << sdp_text;
  std::cout << std::endl;
}
