
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
/// \file QtRtpJpegVaapiPayloader.cc
///

#include "qt6/QtRtpJpegVaapiPayloader.h"

namespace mediax::qt6 {

QtRtpJpegVaapiPayloader::QtRtpJpegVaapiPayloader(QObject *parent) : QtRtpPayloader(parent) {}

QtRtpJpegVaapiPayloader::~QtRtpJpegVaapiPayloader() = default;

Q_INVOKABLE void QtRtpJpegVaapiPayloader::setStreamInfo(const mediax::rtp::StreamInformation &stream_information) {
  payloader_.SetStreamInfo(stream_information);
}

Q_INVOKABLE bool QtRtpJpegVaapiPayloader::open() { return payloader_.Open(); }

Q_INVOKABLE void QtRtpJpegVaapiPayloader::start() { payloader_.Start(); }

Q_INVOKABLE void QtRtpJpegVaapiPayloader::stop() { payloader_.Stop(); }

Q_INVOKABLE void QtRtpJpegVaapiPayloader::close() { payloader_.Close(); }

Q_INVOKABLE int QtRtpJpegVaapiPayloader::transmit(Frame *frame, bool blocking) {
  return payloader_.Transmit(reinterpret_cast<uint8_t *>(frame->video.data()), blocking);
}

void QtRtpJpegVaapiPayloader::sendFrame(Frame frame) {
  payloader_.Transmit(reinterpret_cast<uint8_t *>(frame.video.data()), true);
}

}  // namespace mediax::qt6
