//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief Functions to convert between different colour spaces
///
/// \file rtp_payloader.cc
///

#include "rtp/rtp_payloader.h"

#include <string>

namespace mediax::rtp {

mediax::rtp::ColourspaceType RtpPayloader::GetColourSpace() const { return egress_.encoding; }

uint32_t RtpPayloader::GetHeight() const { return egress_.height; }

uint32_t RtpPayloader::GetWidth() const { return egress_.width; }

uint32_t RtpPayloader::GetFrameRate() const { return egress_.framerate; }

std::string RtpPayloader::GetIpAddress() const { return egress_.hostname; }

uint32_t RtpPayloader::GetPort() const { return egress_.port_no; }

void RtpPayloader::Start() {
  // Nothing to do for base class
}

void RtpPayloader::Stop() {
  // Nothing to do for base class
}

::mediax::rtp::RtpPortType &RtpPayloader::GetEgressPort() { return egress_; }

}  // namespace mediax::rtp
