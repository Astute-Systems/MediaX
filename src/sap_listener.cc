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
/// \file sap_listener.cc

#include "sap_listener.h"

#include <chrono>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace sap {

SAPListener::SAPListener() = default;

SAPListener::~SAPListener() = default;

SAPListener &SAPListener::GetInstance() {
  static SAPListener singleton_;
  return singleton_;
}

const std::vector<SDPMessage> &SAPListener::GetSAPAnnouncements() { return announcements_; }

}  // namespace sap