//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \file rtp_receive.cpp
///

/// [QtReceive example code]
#include "rtp_receive.h"

#include "rtp/rtp_utils.h"

QtReceive::QtReceive() {
  std::cout << "Qt6 Example RTP (Tx) streaming (640x480 Uncompressed YUV) to 127.0.0.1:5004@25Htz\n";
  mediax::rtp::StreamInformation stream_info = {
      "qt-test", "127.0.0.1", 5004, 480, 640, 25, ::mediax::rtp::ColourspaceType::kColourspaceYuv422, false};
  rtp.setStreamInfo(stream_info);
  rtp.open();
  rtp.start();

  // Connect the frame signal to the slot
  connect(this, &QtReceive::newFrame, &rtp, &mediax::qt6::QtRtpUncompressedDepayloader::newFrame);
}

QtReceive::~QtReceive() {
  rtp.stop();
  rtp.close();
}

void QtReceive::newFrame(Frame frame) { std::cout << "New frame received: " << frame_count++ << "\n"; }
/// [QtReceive example code]
