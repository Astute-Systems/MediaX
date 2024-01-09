
//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video class for uncompressed DEF-STAN 00-82 video streams
///
/// \file QtRtpH265Payloader.cc
///

#include "qt6/QtRtpH265Payloader.h"

namespace mediax::qt6 {

QtRtpH265Payloader::QtRtpH265Payloader(QObject *parent) : QtRtpPayloader(parent) {}

QtRtpH265Payloader::~QtRtpH265Payloader() = default;

Q_INVOKABLE void QtRtpH265Payloader::setStreamInfo(const mediax::rtp::StreamInformation &stream_information) {
  payloader_.SetStreamInfo(stream_information);
}

Q_INVOKABLE bool QtRtpH265Payloader::open() { return payloader_.Open(); }

Q_INVOKABLE void QtRtpH265Payloader::start() { payloader_.Start(); }

Q_INVOKABLE void QtRtpH265Payloader::stop() { payloader_.Stop(); }

Q_INVOKABLE void QtRtpH265Payloader::close() { payloader_.Close(); }

Q_INVOKABLE int QtRtpH265Payloader::transmit(Frame *frame, bool blocking) {
  return payloader_.Transmit(reinterpret_cast<uint8_t *>(frame->video.data()), blocking);
}

void QtRtpH265Payloader::sendFrame(Frame frame) {
  payloader_.Transmit(reinterpret_cast<uint8_t *>(frame.video.data()), true);
}

}  // namespace mediax::qt6
