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
/// \file QtRtpOpenH264Depayloader.cc
///

#include "qt6/QtRtpVaapiH264Depayloader.h"

#include <algorithm>

namespace mediax::qt6 {

QtRtpVaapiH264Depayloader::QtRtpVaapiH264Depayloader(QObject *parent) : QtRtpDepayloader(parent) {}

void QtRtpVaapiH264Depayloader::setStreamInfo(const mediax::rtp::StreamInformation &stream_information) {
  m_depayloader.SetStreamInfo(stream_information);

  // Register callback to emit new frame
  m_depayloader.RegisterCallback(
      [this](const mediax::rtp::RtpDepayloader &depay [[maybe_unused]], mediax::rtp::RtpFrameData frame) {
        Frame frame_data;

        int bits_per_pixel = mediax::BitsPerPixel(frame.encoding);
        frame_data.video.resize(frame.resolution.height * frame.resolution.width * bits_per_pixel);
        memcpy(frame_data.video.data(), frame.cpu_buffer,
               frame.resolution.height * frame.resolution.width * bits_per_pixel);

        frame_data.height = m_depayloader.GetHeight();
        frame_data.width = m_depayloader.GetWidth();
        frame_data.encoding = frame.encoding;

        emit newFrame(frame_data);
      });
}

Q_INVOKABLE bool QtRtpVaapiH264Depayloader::open() { return m_depayloader.Open(); }

Q_INVOKABLE void QtRtpVaapiH264Depayloader::start() { m_depayloader.Start(); }

Q_INVOKABLE void QtRtpVaapiH264Depayloader::stop() { m_depayloader.Stop(); }

Q_INVOKABLE void QtRtpVaapiH264Depayloader::close() { m_depayloader.Close(); }

Q_INVOKABLE bool QtRtpVaapiH264Depayloader::receive(QByteArray *frame, int timeout) {
  mediax::rtp::RtpFrameData frame_data;
  bool result = m_depayloader.Receive(&frame_data, timeout);
  if (result) {
    *frame = QByteArray(reinterpret_cast<const char *>(frame_data.cpu_buffer), m_depayloader.GetBuffer().size());
  }
  return result;
}

Q_INVOKABLE QVector<quint8> QtRtpVaapiH264Depayloader::getBuffer() {
  QVector<quint8> buffer(m_depayloader.GetBuffer().size());
  // Copy buffer
  std::copy(m_depayloader.GetBuffer().begin(), m_depayloader.GetBuffer().end(), buffer.begin());
  return buffer;
}

Q_INVOKABLE mediax::rtp::StreamState QtRtpVaapiH264Depayloader::getState() const { return m_depayloader.GetState(); }

}  // namespace mediax::qt6
