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
/// \file rtp_depayloader.cc
///

#include "rtp/rtp_depayloader.h"

#include <string_view>

namespace mediax {

void RtpDepayloader::SetSessionName(std::string_view name) { ingress_.name = name; }

std::string RtpDepayloader::GetSessionName() const { return ingress_.name; }

ColourspaceType RtpDepayloader::GetColourSpace() const { return ingress_.encoding; }

void RtpDepayloader::SetHeight(uint32_t height) { ingress_.height = height; }

uint32_t RtpDepayloader::GetHeight() const { return ingress_.height; }

void RtpDepayloader::SetWidth(uint32_t width) { ingress_.width = width; }

uint32_t RtpDepayloader::GetWidth() const { return ingress_.width; }

void RtpDepayloader::SetFramerate(uint32_t framerate) { ingress_.framerate = framerate; }

uint32_t RtpDepayloader::GetFrameRate() const { return ingress_.framerate; }

std::string RtpDepayloader::GetIpAddress() const { return ingress_.hostname; }

void RtpDepayloader::SetIpAddress(std::string_view ip_address) { ingress_.hostname = ip_address; }

void RtpDepayloader::SetPort(uint32_t port) { ingress_.port_no = port; }

uint32_t RtpDepayloader::GetPort() const { return ingress_.port_no; }

}  // namespace mediax
