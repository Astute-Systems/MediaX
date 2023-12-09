
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
/// \file QtRtpJpegPayloader.cc
///

#include "qt6/QtRtpJpegPayloader.h"

namespace mediax::qt6 {

QtRtpJpegPayloader::QtRtpJpegPayloader(QObject *parent) : QtRtpPayloader(parent) {}

QtRtpJpegPayloader::~QtRtpJpegPayloader() = default;

Q_INVOKABLE void QtRtpJpegPayloader::setStreamInfo(const mediax::rtp::StreamInformation &stream_information) {
  payloader_.SetStreamInfo(stream_information);
}

Q_INVOKABLE bool QtRtpJpegPayloader::open() { return payloader_.Open(); }

Q_INVOKABLE void QtRtpJpegPayloader::start() { payloader_.Start(); }

Q_INVOKABLE void QtRtpJpegPayloader::stop() { payloader_.Stop(); }

Q_INVOKABLE void QtRtpJpegPayloader::close() { payloader_.Close(); }

Q_INVOKABLE int QtRtpJpegPayloader::transmit(Frame *frame, bool blocking) {
  return payloader_.Transmit(reinterpret_cast<uint8_t *>(frame->video.data()), blocking);
}

void QtRtpJpegPayloader::sendFrame(Frame frame) {
  payloader_.Transmit(reinterpret_cast<uint8_t *>(frame.video.data()), true);
}

}  // namespace mediax::qt6
