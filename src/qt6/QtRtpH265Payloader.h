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
/// \file QtRtpH265Payloader.h
///

#ifndef QT6_QTRTPH265PAYLOADER_H_
#define QT6_QTRTPH265PAYLOADER_H_

#include <QByteArray>
#include <QObject>

#include "h265/gst/vaapi/rtp_h265_payloader.h"
#include "qt6/QtCommon.h"
#include "qt6/QtRtpPayloader.h"

namespace mediax::qt6 {

/// The H.265 Qt Payloaader wrapper
class QtRtpH265Payloader : public QtRtpPayloader {
  Q_OBJECT

 public:
  ///
  /// \brief Construct a new Qt Rtp H 2 6 4 Payloader object
  ///
  /// \param parent
  ///
  explicit QtRtpH265Payloader(QObject *parent = nullptr);

  ///
  /// \brief Destroy the Qt Rtp H 2 6 4 Payloader object
  ///
  ///
  ~QtRtpH265Payloader() final;

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
  ::mediax::rtp::h265::gst::vaapi::RtpH265GstVaapiPayloader payloader_;
};

}  // namespace mediax::qt6

#endif  // QT6_QTRTPH265PAYLOADER_H_
