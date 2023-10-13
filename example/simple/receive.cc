//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief A simple video receiver example
///
/// \file receive.cc
///

/// [Receive includes]
#include "rtp/rtp.h"
#include "sap/sap_listener.h"
/// [Receive includes]

int main(int argc, char *argv[]) {
  /// [Receive example open]
  // Initalise the RTP library once on startup
  mediax::InitRtp(argc, argv);
  // Setup RTP streaming class for receive
  mediax::RtpUncompressedDepayloader rtp;
  // Get the SAP/SDP listener singleton instance
  ::mediax::sap::SAPListener &sap = ::mediax::sap::SAPListener::GetInstance();
  // Create a stream information object
  mediax::StreamInformation stream_information = {
      "session_test", "127.0.0.1", 5004, 480, 640, 25, ::mediax::ColourspaceType::kColourspaceRgb24, false};
  // Add a SAP announcement for the new stream
  sap.Start();
  // Create the RTP stream
  rtp.SetStreamInfo(stream_information);
  rtp.Start();
  /// [Receive example open]

  /// [Receive example receive]
  std::vector<uint8_t> cpu_buffer(640 * 480 * ::mediax::BytesPerPixel(stream_information.encoding));
  if (uint8_t *data = cpu_buffer.data(); rtp.Receive(&data, true) == true) {
    std::cerr << "Received data\n";
  } else {
    std::cerr << "Timedout\n";
  }
  /// [Receive example receive]

  /// [Receive example close]
  // Close the SAP session/s
  sap.Stop();
  // Close the RTP session
  rtp.Stop();
  rtp.Stop();
  mediax::RtpCleanup();
  /// [Receive example close]

  return 0;
}
