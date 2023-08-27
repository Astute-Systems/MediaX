//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video class for H.264 DEF-STAN 00-82 video streams
///
/// \file rtph264_payloader.cc
///

#include "h264/rtph264_payloader.h"

#include "h264/rtph264_depayloader.h"

namespace mediax {

RtpH264Payloader::RtpH264Payloader() = default;

RtpH264Payloader::~RtpH264Payloader() {}

void RtpH264Payloader::SetStreamInfo(std::string_view name, ColourspaceType encoding, uint32_t height, uint32_t width,
                                     std::string_view hostname, const uint32_t portno) {
  egress_.encoding = encoding;
  egress_.height = height;
  egress_.width = width;
  egress_.framerate = 25;
  egress_.name = name;
  egress_.hostname = hostname;
  egress_.port_no = portno;
}

int RtpH264Payloader::Transmit(unsigned char*, bool) { return 0; }

bool RtpH264Payloader::Open() { return true; }

void RtpH264Payloader::Close() {}

void RtpH264Payloader::Start() {}

void RtpH264Payloader::Stop() {}

}  // namespace mediax
