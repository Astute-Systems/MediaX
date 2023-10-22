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

#include "rtp_transmit.h"

#include "rtp/rtp_utils.h"

QtTransmit::QtTransmit() {
  std::cout << "Qt6 Example RTP (Tx) streaming (640x480 Uncompressed YUV) to 127.0.0.1:5004@25Htz\n";
  mediax::rtp::StreamInformation stream_info = {
      "qt-test", "127.0.0.1", 5004, 480, 640, 25, ::mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  rtp.setStreamInfo(stream_info);
  rtp.open();
  rtp.start();

  // Connect the frame signal to the slot
  connect(this, &QtTransmit::newFrame, &rtp, &mediax::qt::QtRtpUncompressedPayloader::sendFrame);
}

QtTransmit::~QtTransmit() {
  rtp.stop();
  rtp.close();
}

void QtTransmit::sendFrame() {
  // Create a frame of RGB pixels in QByteArray
  QByteArray frame;
  frame.resize(640 * 480 * 3);

  CreateColourBarEbuTestCard(reinterpret_cast<uint8_t*>(frame.data()), 640, 480,
                             mediax::rtp::ColourspaceType::kColourspaceYuv);
  // Send the frame to the payloader
  emit newFrame(&frame);
  // Update counter
  frame_count++;
  // Print out frame count
  std::cout << "Frame: " << frame_count << "\r" << std::flush;
  // Trigger again in 40ms
  QTimer::singleShot(40, this, SLOT(sendFrame()));
}