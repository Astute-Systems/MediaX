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
/// \file QtRtpH264Depayloader.h
///

#ifndef QT_QTRTPH264PAYLOADER_H_
#define QT_QTRTPH264PAYLOADER_H_

#include <QByteArray>
#include <QObject>
#include <QVector>

#include "rtph264_depayloader.h"

namespace mediax::qt {

class RtpH264DepayloaderWrapper : public QObject {
  Q_OBJECT

 public:
  explicit RtpH264DepayloaderWrapper(QObject *parent = nullptr) : QObject(parent) {}

  Q_INVOKABLE void setStreamInfo(const QString &hostname, int port, const QString &name, const QString &encoding,
                                 int height, int width) {
    StreamInformation stream_information;
    stream_information.hostname = hostname.toStdString();
    stream_information.port = port;
    stream_information.name = name.toStdString();
    stream_information.encoding = encoding.toStdString();
    stream_information.height = height;
    stream_information.width = width;
    depayloader_.SetStreamInfo(stream_information);
  }

  Q_INVOKABLE bool open() { return depayloader_.Open(); }

  Q_INVOKABLE void start() { depayloader_.Start(); }

  Q_INVOKABLE void stop() { depayloader_.Stop(); }

  Q_INVOKABLE void close() { depayloader_.Close(); }

  Q_INVOKABLE bool receive(QByteArray &frame, int timeout = 0) {
    uint8_t *cpu = nullptr;
    bool result = depayloader_.Receive(&cpu, timeout);
    if (result) {
      frame = QByteArray(reinterpret_cast<const char *>(cpu), depayloader_.GetBuffer().size());
    }
    return result;
  }

  Q_INVOKABLE QVector<quint8> getBuffer() const { return QVector<quint8>::fromStdVector(depayloader_.GetBuffer()); }

  Q_INVOKABLE void newFrame() { depayloader_.NewFrame(); }

 private:
  RtpH264Depayloader depayloader_;
};

}  // namespace mediax::qt

#endif  // QT_QTRTPH264PAYLOADER_H_
