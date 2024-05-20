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

#define LIVE 0

int main(int argc, char *argv[]) {
  uint32_t count = 0;
  // Use OMX to get hardware offload on the Raspberry Pi 5
  // mediax::RtpSapTransmit<mediax::rtp::h264::gst::omx::RtpH264GstOmxPayloader> rtp(
  //     "238.192.1.1", 5004, "test-session-name", 640, 480, 25, "H264");

  // Use Open to get software encoding on most platforms
  mediax::RtpSapTransmit<mediax::rtp::h264::gst::open::RtpH264GstOpenPayloader> rtp(
      "238.192.1.1", 5004, "test-session-name", 640, 480, 25, "H264");

  // Convert functions optimised for CPU
  mediax::video::ColourSpaceCpu convert;

  // Converted RGBA buffer
  std::vector<uint8_t> rgb_buffer(640 * 480 * 3);

  while (true) {
#if LIVE
    // Capture a frame from a live video source in YUC420P format (Block till frame recieved)
#else

    // Create a buffer in YUV420P format
    std::vector<uint8_t> &data =
        rtp.GetBufferTestPattern(640, 480, ::mediax::rtp::ColourspaceType::kColourspaceYuv420p, 10);
    // 40ms delay
    usleep(40000);
#endif
    // Convert to the required format
    convert.Yuv420pToRgb(480, 640, data.data(), rgb_buffer.data());

    // Transmit the frame
    rtp.Transmit(rgb_buffer.data(), false);

    // Overwrite line
    std::cout << "Frame " << count++ << "\r";
    std::cout.flush();
  }
}
