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
/// \file rtp_sap_transmit_arm.cc
///

#include "rtp/rtp.h"
int main(int argc, char *argv[]) {
  mediax::RtpSapTransmit<mediax::rtp::h264::gst::omx::RtpH264GstOmxPayloader> rtp(
      "238.192.1.1", 5004, "test-session-name", 640, 480, 25, "H264");
  // Create a buffer in unsupported format
  std::vector<uint8_t> &data = rtp.GetBufferTestPattern(640, 480, ::mediax::rtp::ColourspaceType::kColourspaceYuv422);
  // Convert the buffer to RGB
  mediax::video::ColourSpaceCpu converter;

  while (true) rtp.Transmit(data.data(), false);
}
