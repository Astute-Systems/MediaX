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

#include <arpa/inet.h>

#include <iostream>
#include <string_view>

namespace mediax::rtp {

void RtpDepayloader::SetSessionName(std::string_view name) { ingress_.name = name; }

void RtpDepayloader::RegisterCallback(const ::mediax::rtp::RtpCallback& callback) {
  callback_ = callback;
  callback_registered_ = true;
}

std::string RtpDepayloader::GetSessionName() const { return ingress_.name; }

::mediax::rtp::ColourspaceType RtpDepayloader::GetColourSpace() const { return ingress_.encoding; }

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

void RtpDepayloader::SetColourSpace(::mediax::rtp::ColourspaceType colourspace) { ingress_.encoding = colourspace; }

::mediax::rtp::ColourspaceType RtpDepayloader::GetColourSpace() { return ingress_.encoding; }

bool RtpDepayloader::IsMulticast(std::string_view ip_address) {
  struct in_addr addr;
  if (inet_aton(std::string(ip_address).c_str(), &addr) == 0) {
    return false;
  }
  return IN_MULTICAST(ntohl(addr.s_addr));
}

bool RtpDepayloader::CallbackRegistered() const { return callback_registered_; }

void RtpDepayloader::UnregisterCallback() { callback_registered_ = false; }

::mediax::rtp::RtpPortType& RtpDepayloader::GetStream() { return ingress_; };

}  // namespace mediax::rtp
