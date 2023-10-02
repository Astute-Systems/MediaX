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

#include "h264/gst/rtp_h264_payloader.h"

#include "h264/gst/rtp_h264_depayloader.h"

namespace mediax {

RtpH264Payloader::RtpH264Payloader() = default;

RtpH264Payloader::~RtpH264Payloader() = default;

void RtpH264Payloader::SetStreamInfo(const ::mediax::StreamInformation& stream_information) {
  egress_.encoding = stream_information.encoding;
  egress_.height = stream_information.height;
  egress_.width = stream_information.width;
  egress_.framerate = stream_information.framerate;
  egress_.name = stream_information.session_name;
  egress_.hostname = stream_information.hostname;
  egress_.port_no = stream_information.port;
  egress_.settings_valid = true;
}

int RtpH264Payloader::Transmit(unsigned char*, bool) { return 0; }

bool RtpH264Payloader::Open() {
  // TODO(Ross): implement this function
  return true;
}

void RtpH264Payloader::Close() {
  // TODO(Ross): implement this function
}

void RtpH264Payloader::Start() {
  // TODO(Ross): implement this function
}

void RtpH264Payloader::Stop() {
  // TODO(Ross): implement this function
}

}  // namespace mediax
