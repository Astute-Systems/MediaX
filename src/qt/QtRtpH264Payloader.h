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

#include "h264/gst/vaapi/rtp_h264_payloader.h"

namespace mediax::qt {

class QtRtpH264Payloader : public QObject {
  Q_OBJECT

 public:
  ///
  /// \brief Construct a new Qt Rtp H 2 6 4 Payloader object
  ///
  /// \param parent
  ///
  explicit QtRtpH264Payloader(QObject *parent = nullptr);

  ///
  /// \brief Destroy the Qt Rtp H 2 6 4 Payloader object
  ///
  ///
  ~QtRtpH264Payloader() final;

  ///
  /// \brief Set the Stream Info object
  ///
  /// \param hostname the IPV4 address
  /// \param port the port number
  /// \param name the name of the stream
  /// \param height the height of the stream
  /// \param width the width of the stream
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE void setStreamInfo(const QString &hostname, int port, const QString &name, int height, int width);

  ///
  /// \brief Open the RTP stream
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE bool open();

  ///
  /// \brief Start the RTP stream
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE void start();

  ///
  /// \brief Stop the RTP stream
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE void stop();

  ///
  /// \brief Close the RTP stream
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE void close();

  ///
  /// \brief Transmit a frame to the RTP stream
  ///
  /// \param frame The frame to transmit
  /// \param blocking Set to true if blocking
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE int transmit(QByteArray &frame, bool blocking = true);

 private:
  ::mediax::h264::gst::vaapi::RtpH264Payloader payloader_;
};

}  // namespace mediax::qt

#endif  // QT_QTRTPH264PAYLOADER_H_
