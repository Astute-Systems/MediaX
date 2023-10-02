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

#include "h264/gst/rtp_h264_payloader.h"

namespace mediax::qt {

class RtpH264PayloaderWrapper : public QObject {
  Q_OBJECT

 public:
  explicit RtpH264PayloaderWrapper(QObject *parent = nullptr);

  ~RtpH264PayloaderWrapper() = default;

  Q_INVOKABLE void setStreamInfo(const QString &hostname, int port, const QString &name, int height, int width);

  Q_INVOKABLE bool open();

  Q_INVOKABLE void start();

  Q_INVOKABLE void stop();

  Q_INVOKABLE void close();

  Q_INVOKABLE int transmit(QByteArray &frame, bool blocking = true);  // NOLINT

 private:
  RtpH264Payloader payloader_;
};

}  // namespace mediax::qt

#endif  // QT_QTRTPH264PAYLOADER_H_
