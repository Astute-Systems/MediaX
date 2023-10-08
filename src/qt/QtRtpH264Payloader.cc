
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
/// \file QtRtpH264Payloader.cc
///

#include "qt/QtRtpH264Payloader.h"

namespace mediax::qt {

QtRtpH264Payloader::QtRtpH264Payloader(QObject *parent) : QObject(parent) {}

QtRtpH264Payloader::~QtRtpH264Payloader() = default;

Q_INVOKABLE void QtRtpH264Payloader::setStreamInfo(const QString &hostname, int port, const QString &session_name,
                                                   int height, int width) {
  StreamInformation stream_information;
  stream_information.hostname = hostname.toStdString();
  stream_information.port = port;
  stream_information.session_name = session_name.toStdString();
  stream_information.encoding = mediax::ColourspaceType::kColourspaceH264Part10;
  stream_information.height = height;
  stream_information.width = width;
  payloader_.SetStreamInfo(stream_information);
}

Q_INVOKABLE bool QtRtpH264Payloader::open() { return payloader_.Open(); }

Q_INVOKABLE void QtRtpH264Payloader::start() { payloader_.Start(); }

Q_INVOKABLE void QtRtpH264Payloader::stop() { payloader_.Stop(); }

Q_INVOKABLE void QtRtpH264Payloader::close() { payloader_.Close(); }

Q_INVOKABLE int QtRtpH264Payloader::transmit(QByteArray *frame, bool blocking) {
  return payloader_.Transmit(reinterpret_cast<uint8_t *>(frame->data()), blocking);
}

}  // namespace mediax::qt
