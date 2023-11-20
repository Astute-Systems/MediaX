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
/// \file QtRtpDepayloader.h
///

#ifndef QT_QTRTPDEPAYLOADER_H_
#define QT_QTRTPDEPAYLOADER_H_

#include <QByteArray>
#include <QObject>

#include "qt/QtCommon.h"
#include "rtp/rtp.h"

namespace mediax::qt {

class QtRtpDepayloader : public QObject {
  Q_OBJECT

 public:
  ///
  /// \brief Construct a new Qt Rtp Depayloader object
  ///
  /// \param parent
  ///
  explicit QtRtpDepayloader(QObject *parent = nullptr);

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
  Q_INVOKABLE virtual void setStreamInfo(const mediax::rtp::StreamInformation &stream_information) = 0;

  ///
  /// \brief Open the RTP stream
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE virtual bool open() = 0;

  ///
  /// \brief Start the RTP stream
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE virtual void start() = 0;

  ///
  /// \brief Stop the RTP stream
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE virtual void stop() = 0;

  ///
  /// \brief Close the RTP stream
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE virtual void close() = 0;

  ///
  /// \brief Receive a frame from the RTP stream
  ///
  /// \param frame buffer to hold the recieved image
  /// \param timeout timeout in milliseconds, if zero wait for ever (blocking)
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE virtual bool receive(QByteArray *frame, int timeout = 0) = 0;

  ///
  /// \brief Get the Buffer object
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE virtual QVector<quint8> getBuffer() = 0;

  ///
  /// \brief Get the State object
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE virtual mediax::rtp::StreamState getState() const = 0;

 signals:
  virtual void newFrame(Frame frame);
};

}  // namespace mediax::qt

#endif  // QT_QTRTPDEPAYLOADER_H_
