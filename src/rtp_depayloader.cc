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

#include "rtp_depayloader.h"

PortType RtpDepayloader::ingress_ = {};

void RtpDepayloader::SetSessionName(std::string_view name) const { ingress_.name = name; }

std::string RtpDepayloader::GetSessionName() const { return ingress_.name; }

ColourspaceType RtpDepayloader::GetColourSpace() const { return ingress_.encoding; }

void RtpDepayloader::SetHeight(uint32_t height) const { ingress_.height = height; }

uint32_t RtpDepayloader::GetHeight() const { return ingress_.height; }

void RtpDepayloader::SetWidth(uint32_t width) const { ingress_.width = width; }

uint32_t RtpDepayloader::GetWidth() const { return ingress_.width; }

void RtpDepayloader::SetFramerate(uint32_t framerate) const { ingress_.framerate = framerate; }

uint32_t RtpDepayloader::GetFrameRate() const { return ingress_.framerate; }

std::string RtpDepayloader::GetIpAddress() const { return ingress_.hostname; }

void RtpDepayloader::SetIpAddress(std::string_view ip_address) const { ingress_.hostname = ip_address; }

void RtpDepayloader::SetPort(uint32_t port) const { ingress_.port_no = port; }

uint32_t RtpDepayloader::GetPort() const { return ingress_.port_no; }