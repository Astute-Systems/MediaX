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
/// \file QtRtpH264Payloader.h
///

#ifndef QT_QTRTPH264PAYLOADER_H_
#define QT_QTRTPH264PAYLOADER_H_

#include <QByteArray>
#include <QObject>

#include "rtph264_payloader.h"

namespace mediax::qt {

class RtpH264PayloaderWrapper : public QObject {
  Q_OBJECT

 public:
  explicit RtpH264PayloaderWrapper(QObject *parent = nullptr) : QObject(parent) {}

  Q_INVOKABLE void setStreamInfo(const QString &hostname, int port, const QString &name, const QString &encoding,
                                 int height, int width) {
    StreamInformation stream_information;
    stream_information.hostname = hostname.toStdString();
    stream_information.port = port;
    stream_information.name = name.toStdString();
    stream_information.encoding = encoding.toStdString();
    stream_information.height = height;
    stream_information.width = width;
    payloader_.SetStreamInfo(stream_information);
  }

  Q_INVOKABLE bool open() { return payloader_.Open(); }

  Q_INVOKABLE void start() { payloader_.Start(); }

  Q_INVOKABLE void stop() { payloader_.Stop(); }

  Q_INVOKABLE void close() { payloader_.Close(); }

  Q_INVOKABLE int transmit(QByteArray &frame, bool blocking = true) {
    return payloader_.Transmit(reinterpret_cast<uint8_t *>(frame.data()), blocking);
  }

 private:
  RtpH264Payloader payloader_;
};

}  // namespace mediax::qt

#endif  // QT_QTRTPH264PAYLOADER_H_