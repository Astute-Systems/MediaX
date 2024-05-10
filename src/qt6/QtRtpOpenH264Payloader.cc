
//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video class for uncompressed DEF-STAN 00-82 video streams
///
/// \file QtRtpOpenH264Payloader.cc
///

#include "qt6/QtRtpOpenH264Payloader.h"

namespace mediax::qt6 {

QtRtpOpenH264Payloader::QtRtpOpenH264Payloader(QObject *parent) : QtRtpPayloader(parent) {}

QtRtpOpenH264Payloader::~QtRtpOpenH264Payloader() = default;

Q_INVOKABLE void QtRtpOpenH264Payloader::setStreamInfo(const mediax::rtp::StreamInformation &stream_information) {
  payloader_.SetStreamInfo(stream_information);
}

Q_INVOKABLE bool QtRtpOpenH264Payloader::open() { return payloader_.Open(); }

Q_INVOKABLE void QtRtpOpenH264Payloader::start() { payloader_.Start(); }

Q_INVOKABLE void QtRtpOpenH264Payloader::stop() { payloader_.Stop(); }

Q_INVOKABLE void QtRtpOpenH264Payloader::close() { payloader_.Close(); }

Q_INVOKABLE int QtRtpOpenH264Payloader::transmit(Frame *frame, bool blocking) {
  return payloader_.Transmit(reinterpret_cast<uint8_t *>(frame->video.data()), blocking);
}

void QtRtpOpenH264Payloader::sendFrame(Frame frame) {
  payloader_.Transmit(reinterpret_cast<uint8_t *>(frame.video.data()), true);
}

}  // namespace mediax::qt6
