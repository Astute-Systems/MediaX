//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video class for uncompressed DEF-STAN 00-82 video streams
///
/// \file QtRtpJpegVaapiDepayloader.h
///

#ifndef QT6_QTRTPJPEGVAAPIDEPAYLOADER_H_
#define QT6_QTRTPJPEGVAAPIDEPAYLOADER_H_

#include <QByteArray>
#include <QObject>
#include <QVector>

#include "jpeg/gst/rtp_jpeg_depayloader.h"
#include "qt6/QtRtpDepayloader.h"

namespace mediax::qt6 {

/// A RTP depayloader for H.265 DEF-STAN 00-82 video streams
class QtRtpJpegVaapiDepayloader : public QtRtpDepayloader {
 public:
  ///
  /// \brief Construct a new Qt Rtp H 2 6 4 Deayloader object
  ///
  /// \param parent
  ///
  explicit QtRtpJpegVaapiDepayloader(QObject *parent = nullptr);

  ///
  /// \brief Set the Stream Info object
  ///
  /// \param  stream_information set the stream information
  ///
  Q_INVOKABLE void setStreamInfo(const mediax::rtp::StreamInformation &stream_information) final;

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
  ///
  Q_INVOKABLE bool receive(QByteArray *frame, int timeout = 0) final;

  ///
  /// \brief Get the Buffer object
  ///
  ///
  Q_INVOKABLE QVector<quint8> getBuffer() final;

  ///
  /// \brief Get the State object
  ///
  ///
  Q_INVOKABLE mediax::rtp::StreamState getState() const final;

 private:
  /// The underlying RTP H.265 depayloader
  mediax::rtp::jpeg::gst::vaapi::RtpJpegGstVaapiDepayloader m_depayloader;
};

}  // namespace mediax::qt6

#endif  // QT6_QTRTPJPEGVAAPIDEPAYLOADER_H_
