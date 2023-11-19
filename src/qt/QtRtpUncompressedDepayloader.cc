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
/// \file QtRtpUncompressedDepayloader.cc
///

#include "qt/QtRtpUncompressedDepayloader.h"

#include "rtp/rtp_depayloader.h"
 #include <algorithm> 

namespace mediax::qt {

QtRtpUncompressedDepayloader::QtRtpUncompressedDepayloader(QObject* parent) : QtRtpDepayloader(parent) {}

void QtRtpUncompressedDepayloader::setStreamInfo(const QString& hostname, int port, const QString& session_name,
                                                 int height, int width, int framerate) {
  ::mediax::rtp::StreamInformation stream_information;
  stream_information.hostname = hostname.toStdString();
  stream_information.port = port;
  stream_information.session_name = session_name.toStdString();
  stream_information.height = height;
  stream_information.width = width;
  stream_information.framerate = framerate;
  stream_information.encoding = mediax::rtp::ColourspaceType::kColourspaceRgb24;
  m_depayloader.SetStreamInfo(stream_information);

  // Register callback to emit new frame
  m_depayloader.RegisterCallback([this](const mediax::rtp::RtpDepayloader& depay, mediax::rtp::RtpCallbackData frame) {
    QByteArray frame_data(reinterpret_cast<const char*>(frame.cpu_buffer),
                          frame.resolution.height * frame.resolution.width * 3);

    emit newFrame(&frame_data);
  });
}

Q_INVOKABLE bool QtRtpUncompressedDepayloader::open() { return m_depayloader.Open(); }

Q_INVOKABLE void QtRtpUncompressedDepayloader::start() { m_depayloader.Start(); }

Q_INVOKABLE void QtRtpUncompressedDepayloader::stop() { m_depayloader.Stop(); }

Q_INVOKABLE void QtRtpUncompressedDepayloader::close() { m_depayloader.Close(); }

Q_INVOKABLE bool QtRtpUncompressedDepayloader::receive(QByteArray* frame, int timeout) {
  uint8_t* cpu = nullptr;
  bool result = m_depayloader.Receive(&cpu, timeout);
  if (result) {
    *frame = QByteArray(reinterpret_cast<const char*>(cpu), m_depayloader.GetBuffer().size());
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

}  // namespace mediax::qt
