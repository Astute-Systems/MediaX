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

#include "h264/gst/vaapi/rtp_h264_depayloader.h"
#include "rtp/rtp_depayloader.h"

namespace mediax::qt {

class QtRtpH264Depayloader : public QObject {
  Q_OBJECT

 public:
  ///
  /// \brief Construct a new Qt Rtp H 2 6 4 Deayloader object
  ///
  /// \param parent
  ///
  explicit QtRtpH264Depayloader(QObject *parent = nullptr);

  ///
  /// \brief Set the Stream Info object
  ///
  /// \param hostname
  /// \param port
  /// \param session_name
  /// \param height
  /// \param width
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE void setStreamInfo(const QString &hostname, int port, const QString &session_name, int height, int width);

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
  /// \brief Receive a frame from the RTP stream
  ///
  /// \param frame buffer to hold the recieved image
  /// \param timeout timeout in milliseconds, if zero wait for ever (blocking)
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE bool receive(QByteArray *frame, int timeout = 0);

  ///
  /// \brief Get the Buffer object
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE QVector<quint8> getBuffer();

  ///
  /// \brief Get the State object
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE mediax::rtp::StreamState getState();

 private:
  /// The underlying RTP H.264 depayloader
  mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader depayloader_;
};

}  // namespace mediax::qt

#endif  // QT_QTRTPH264DEPAYLOADER_H_
