
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
/// \file QtRtpAv1Payloader.cc
///

#include "qt6/QtRtpAv1Payloader.h"

namespace mediax::qt6 {

QtRtpAv1Payloader::QtRtpAv1Payloader(QObject *parent) : QtRtpPayloader(parent) {}

QtRtpAv1Payloader::~QtRtpAv1Payloader() = default;

Q_INVOKABLE void QtRtpAv1Payloader::setStreamInfo(const mediax::rtp::StreamInformation &stream_information) {
  payloader_.SetStreamInfo(stream_information);
}

Q_INVOKABLE bool QtRtpAv1Payloader::open() { return payloader_.Open(); }

Q_INVOKABLE void QtRtpAv1Payloader::start() { payloader_.Start(); }

Q_INVOKABLE void QtRtpAv1Payloader::stop() { payloader_.Stop(); }

Q_INVOKABLE void QtRtpAv1Payloader::close() { payloader_.Close(); }

Q_INVOKABLE int QtRtpAv1Payloader::transmit(Frame *frame, bool blocking) {
  return payloader_.Transmit(reinterpret_cast<uint8_t *>(frame->video.data()), blocking);
}

void QtRtpAv1Payloader::sendFrame(Frame frame) {
  payloader_.Transmit(reinterpret_cast<uint8_t *>(frame.video.data()), true);
}

}  // namespace mediax::qt6
