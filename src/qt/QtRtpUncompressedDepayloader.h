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
/// \file QtRtpUncompressedDepayloader.h
///

#ifndef QT_QTRTPUNCOMPRESSEDDEPAYLOADER_H_
#define QT_QTRTPUNCOMPRESSEDDEPAYLOADER_H_

#include <QObject>
#include <memory>

#include "qt/QtRtpDepayloader.h"
#include "uncompressed/rtp_uncompressed_depayloader.h"

namespace mediax::qt {

/// A RTP depayloader base class for uncompressed video streams
class QtRtpUncompressedDepayloader : public QtRtpDepayloader {
  Q_OBJECT

 public:
  ///
  /// \brief Construct a new Rtp Uncompressed Depayloader object
  ///
  /// \param parent
  ///
  explicit QtRtpUncompressedDepayloader(QObject* parent = nullptr);

  ///
  /// \brief Set the Stream Info object
  ///
  /// \param stream_information the stream information
  ///
  Q_INVOKABLE void setStreamInfo(const mediax::rtp::StreamInformation& stream_information) final;

  ///
  /// \brief Open the RTP stream
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE bool open() final;

  ///
  /// \brief Start the RTP stream
  ///
  ///
  Q_INVOKABLE void start() final;

  ///
  /// \brief Stop the RTP stream
  ///
  ///
  Q_INVOKABLE void stop() final;

  ///
  /// \brief Close the RTP stream
  ///
  ///
  Q_INVOKABLE void close() final;

  ///
  /// \brief Receive a frame from the RTP stream
  ///
  /// \param frame buffer to hold the recieved image
  /// \param timeout timeout in milliseconds, if zero wait for ever (blocking)
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE bool receive(QByteArray* frame, int timeout = 0) final;

  ///
  /// \brief Get the Buffer object
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE QVector<quint8> getBuffer() final;

  ///
  /// \brief Get the State object
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE mediax::rtp::StreamState getState() const final;

 private:
  /// The underlying RTP depayloader
  mediax::rtp::uncompressed::RtpUncompressedDepayloader m_depayloader;
};

}  // namespace mediax::qt

#endif  // QT_QTRTPUNCOMPRESSEDDEPAYLOADER_H_
