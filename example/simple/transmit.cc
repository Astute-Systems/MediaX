//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief A very simple video transmitter example
///
/// \file transmit.cc
///

/// [Transmit includes]
#include "rtp/rtp.h"
#include "sap/sap_announcer.h"
/// [Transmit includes]

int main(int argc, char** argv) {
  /// [Transmit example open]
  // Initalise the RTP library once on startup
  mediax::InitRtp(argc, argv);
  // Setup RTP streaming class for transmit
  mediax::RtpUncompressedPayloader rtp;
  // Get the SAP/SDP announcment singleton instance
  mediax::sap::SAPAnnouncer& sap = mediax::sap::SAPAnnouncer::GetInstance();
  // Create a stream information object
  mediax::StreamInformation stream_information = {
      "session_test", "127.0.0.1", 5004, 480, 640, 25, ::mediax::ColourspaceType::kColourspaceRgb24, false};
  // Add a SAP announcement for the new stream
  sap.AddSapAnnouncement(stream_information);
  // Start the SAP announcer thread, will now be emitted once a second
  sap.Start();
  // Create the RTP stream
  rtp.SetStreamInfo(stream_information);
  rtp.Open();
  // Start the new stream
  rtp.Start();
  /// [Transmit example open]

  /// [Transmit example transmit]
  std::vector<uint8_t> transmit_buffer(640 * 480 * ::mediax::BytesPerPixel(stream_information.encoding));
  // Put something in the buffer here
  if (rtp.Transmit(transmit_buffer.data(), true) < 0) {
    std::cerr << "Failed to transmit RTP packet.";
  }
  /// [Transmit example transmit]

  /// [Transmit example close]
  // Close the SAP session/s
  sap.Stop();
  // Close the RTP session
  rtp.Stop();
  rtp.Close();
  mediax::RtpCleanup();
  /// [Transmit example close]

  return 0;
}
