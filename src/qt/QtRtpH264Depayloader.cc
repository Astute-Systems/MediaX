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
/// \file QtRtpH264Depayloader.cc
///

#include "qt/QtRtpH264Depayloader.h"

#include <algorithm>

namespace mediax::qt {

QtRtpH264Depayloader::QtRtpH264Depayloader(QObject *parent) : QtRtpDepayloader(parent) {}

void QtRtpH264Depayloader::setStreamInfo(const mediax::rtp::StreamInformation &stream_information) {
  m_depayloader.SetStreamInfo(stream_information);

  // Register callback to emit new frame
  m_depayloader.RegisterCallback([this](const mediax::rtp::RtpDepayloader &depay, mediax::rtp::RtpCallbackData frame) {
    QByteArray frame_data(reinterpret_cast<const char *>(frame.cpu_buffer),
                          frame.resolution.height * frame.resolution.width * 3);

    emit newFrame(&frame_data);
  });
}

Q_INVOKABLE bool QtRtpH264Depayloader::open() { return m_depayloader.Open(); }

Q_INVOKABLE void QtRtpH264Depayloader::start() { m_depayloader.Start(); }

Q_INVOKABLE void QtRtpH264Depayloader::stop() { m_depayloader.Stop(); }

Q_INVOKABLE void QtRtpH264Depayloader::close() { m_depayloader.Close(); }

Q_INVOKABLE bool QtRtpH264Depayloader::receive(QByteArray *frame, int timeout) {
  uint8_t *cpu = nullptr;
  bool result = m_depayloader.Receive(&cpu, timeout);
  if (result) {
    *frame = QByteArray(reinterpret_cast<const char *>(cpu), m_depayloader.GetBuffer().size());
  }
  return result;
}

Q_INVOKABLE QVector<quint8> QtRtpH264Depayloader::getBuffer() {
  QVector<quint8> buffer(m_depayloader.GetBuffer().size());
  // Copy buffer
  std::copy(m_depayloader.GetBuffer().begin(), m_depayloader.GetBuffer().end(), buffer.begin());
  return buffer;
}

Q_INVOKABLE mediax::rtp::StreamState QtRtpH264Depayloader::getState() const { return m_depayloader.GetState(); }

}  // namespace mediax::qt
