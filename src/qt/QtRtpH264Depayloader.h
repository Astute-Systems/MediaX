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

#ifndef QT_QTRTPH264DEPAYLOADER_H_
#define QT_QTRTPH264DEPAYLOADER_H_

#include <QByteArray>
#include <QObject>
#include <QVector>

#include "h264/gst/rtp_h264_depayloader.h"

namespace mediax::qt {

class RtpH264DepayloaderWrapper : public QObject {
  Q_OBJECT

 public:
  explicit RtpH264DepayloaderWrapper(QObject *parent = nullptr);

  Q_INVOKABLE void setStreamInfo(const QString &hostname, int port, const QString &session_name, int height, int width);

  Q_INVOKABLE bool open();

  Q_INVOKABLE void start();

  Q_INVOKABLE void stop();

  Q_INVOKABLE void close();

  Q_INVOKABLE bool receive(QByteArray &frame, int timeout = 0);  // NOLINT

  Q_INVOKABLE QVector<quint8> getBuffer() const;

  Q_INVOKABLE void newFrame();

 private:
  RtpH264Depayloader depayloader_;
};

}  // namespace mediax::qt

#endif  // QT_QTRTPH264DEPAYLOADER_H_
