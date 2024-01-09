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
/// \file QtRtpH264Payloader.h
///

#ifndef QT6_QTRTPH264PAYLOADER_H_
#define QT6_QTRTPH264PAYLOADER_H_

#include <QByteArray>
#include <QObject>

#include "h264/gst/vaapi/rtp_h264_payloader.h"
#include "qt6/QtCommon.h"
#include "qt6/QtRtpPayloader.h"

namespace mediax::qt6 {

/// The H.264 Qt Payloaader wrapper
class QtRtpH264Payloader : public QtRtpPayloader {
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
  /// \param stream_information set the stream information
  ///
  Q_INVOKABLE void setStreamInfo(const mediax::rtp::StreamInformation &stream_information) final;

  ///
  /// \brief Open the RTP stream
  ///
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
  /// \brief Transmit a frame to the RTP stream
  ///
  /// \param frame The frame to transmit
  /// \param blocking Set to true if blocking
  ///
  Q_INVOKABLE int transmit(Frame *frame, bool blocking = true) final;

 public slots:

  ///
  /// \brief A frame to transmit
  ///
  /// \param frame The frame to receive
  ///
  void sendFrame(Frame frame) final;

 private:
  /// The GStreamer payloader
  ::mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiPayloader payloader_;
};

}  // namespace mediax::qt6

#endif  // QT6_QTRTPH264PAYLOADER_H_
