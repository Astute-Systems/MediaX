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

QtRtpH264Depayloader::QtRtpH264Depayloader(QObject *parent) : QObject(parent) {}

void QtRtpH264Depayloader::setStreamInfo(const QString &hostname, int port, const QString &session_name, int height,
                                         int width) {
  rtp::StreamInformation stream_information;
  stream_information.hostname = hostname.toStdString();
  stream_information.port = port;
  stream_information.session_name = session_name.toStdString();
  stream_information.encoding = mediax::rtp::ColourspaceType::kColourspaceH264Part10;
  stream_information.height = height;
  stream_information.width = width;
  depayloader_.SetStreamInfo(stream_information);
}

Q_INVOKABLE bool QtRtpH264Depayloader::open() { return depayloader_.Open(); }

Q_INVOKABLE void QtRtpH264Depayloader::start() { depayloader_.Start(); }

Q_INVOKABLE void QtRtpH264Depayloader::stop() { depayloader_.Stop(); }

Q_INVOKABLE void QtRtpH264Depayloader::close() { depayloader_.Close(); }

Q_INVOKABLE bool QtRtpH264Depayloader::receive(QByteArray *frame, int timeout) {
  uint8_t *cpu = nullptr;
  bool result = depayloader_.Receive(&cpu, timeout);
  if (result) {
    *frame = QByteArray(reinterpret_cast<const char *>(cpu), depayloader_.GetBuffer().size());
  }
  return result;
}

Q_INVOKABLE QVector<quint8> QtRtpH264Depayloader::getBuffer() {
  QVector<quint8> buffer(depayloader_.GetBuffer().size());
  // Copy buffer
  std::copy(depayloader_.GetBuffer().begin(), depayloader_.GetBuffer().end(), buffer.begin());
  return buffer;
}

Q_INVOKABLE mediax::rtp::StreamState QtRtpH264Depayloader::getState() const { return depayloader_.GetState(); }

}  // namespace mediax::qt
