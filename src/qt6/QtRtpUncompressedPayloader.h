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
/// \file QtRtpUncompressedPayloader.h
///

#ifndef QT_QTRTPUNCOMPRESSEDPAYLOADER_H_
#define QT_QTRTPUNCOMPRESSEDPAYLOADER_H_

#include <QObject>
#include <memory>

#include "qt6/QtCommon.h"
#include "qt6/QtRtpH264Payloader.h"
#include "uncompressed/rtp_uncompressed_payloader.h"

namespace mediax::qt6 {

/// A RTP payloader base class for uncompressed video streams
class QtRtpUncompressedPayloader : public QtRtpPayloader {
  Q_OBJECT

 public:
  ///
  /// \brief Construct a new Qt Rtp Uncompressed Payloader object
  ///
  /// \param parent
  ///
  explicit QtRtpUncompressedPayloader(QObject* parent = nullptr);

  ///
  /// \brief Set the Stream Info object
  ///
  /// \param stream_information
  ///
  void setStreamInfo(const mediax::rtp::StreamInformation& stream_information) final;

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
  Q_INVOKABLE int transmit(Frame* frame, bool blocking = true) final;

 public slots:

  ///
  /// \brief A frame to transmit
  ///
  /// \param frame The frame to receive
  ///
  void sendFrame(Frame frame) final;

 private:
  /// The underlying RTP payloader
  mediax::rtp::uncompressed::RtpUncompressedPayloader m_rtpPayloader;
};

}  // namespace mediax::qt6

#endif  // QT_QTRTPUNCOMPRESSEDPAYLOADER_H_
