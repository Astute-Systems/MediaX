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
/// \file QtRtpUncompressedDepayloader.cc
///

#include "qt6/QtRtpUncompressedDepayloader.h"

#include <algorithm>

#include "rtp/rtp.h"

namespace mediax::qt6 {

QtRtpUncompressedDepayloader::QtRtpUncompressedDepayloader(QObject* parent) : QtRtpDepayloader(parent) {}

void QtRtpUncompressedDepayloader::setStreamInfo(const mediax::rtp::StreamInformation& stream_information) {
  m_depayloader.SetStreamInfo(stream_information);

  // Register callback to emit new frame
  m_depayloader.RegisterCallback(
      [this](const mediax::rtp::RtpDepayloader& depay [[maybe_unused]], mediax::rtp::RtpFrameData frame) {
        Frame frame_data;
        frame_data.video.resize(frame.resolution.height * frame.resolution.width * 3);
        memcpy(frame_data.video.data(), frame.cpu_buffer, frame.resolution.height * frame.resolution.width * 3);

        frame_data.height = frame.resolution.height;
        frame_data.width = frame.resolution.width;
        frame_data.encoding = frame.encoding;

        emit newFrame(frame_data);
      });
}

Q_INVOKABLE bool QtRtpUncompressedDepayloader::open() { return m_depayloader.Open(); }

Q_INVOKABLE void QtRtpUncompressedDepayloader::start() { m_depayloader.Start(); }

Q_INVOKABLE void QtRtpUncompressedDepayloader::stop() { m_depayloader.Stop(); }

Q_INVOKABLE void QtRtpUncompressedDepayloader::close() { m_depayloader.Close(); }

Q_INVOKABLE bool QtRtpUncompressedDepayloader::receive(QByteArray* frame, int timeout) {
  mediax::rtp::RtpFrameData frame_data;
  bool result = m_depayloader.Receive(&frame_data, timeout);
  if (result) {
    *frame = QByteArray(reinterpret_cast<const char*>(frame_data.cpu_buffer), m_depayloader.GetBuffer().size());
  }
  return result;
}

Q_INVOKABLE mediax::rtp::StreamState QtRtpUncompressedDepayloader::getState() const { return m_depayloader.GetState(); }

Q_INVOKABLE QVector<quint8> QtRtpUncompressedDepayloader::getBuffer() {
  QVector<quint8> buffer(m_depayloader.GetBuffer().size());
  // Copy buffer
  std::copy(m_depayloader.GetBuffer().begin(), m_depayloader.GetBuffer().end(), buffer.begin());
  return buffer;
}

}  // namespace mediax::qt6
