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
/// \file QtRtpUncompressedPayloader.cc
///

#include "qt/QtRtpUncompressedPayloader.h"

namespace mediax::qt {
QtRtpUncompressedPayloader::QtRtpUncompressedPayloader(QObject *parent) : QtRtpPayloader(parent) {}

void QtRtpUncompressedPayloader::setStreamInfo(const mediax::rtp::StreamInformation &stream_information) {
  m_rtpPayloader.SetStreamInfo(stream_information);
}

Q_INVOKABLE bool QtRtpUncompressedPayloader::open() { return m_rtpPayloader.Open(); }

Q_INVOKABLE void QtRtpUncompressedPayloader::start() { m_rtpPayloader.Start(); }

Q_INVOKABLE void QtRtpUncompressedPayloader::stop() { m_rtpPayloader.Stop(); }

Q_INVOKABLE void QtRtpUncompressedPayloader::close() { m_rtpPayloader.Close(); }

Q_INVOKABLE int QtRtpUncompressedPayloader::transmit(Frame *frame, bool blocking) {
  return m_rtpPayloader.Transmit(reinterpret_cast<uint8_t *>(frame->video.data()), blocking);
}

void QtRtpUncompressedPayloader::sendFrame(Frame frame) {
  m_rtpPayloader.Transmit(reinterpret_cast<uint8_t *>(frame.video.data()), true);
}

}  // namespace mediax::qt
