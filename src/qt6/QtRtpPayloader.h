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
/// \file QtRtpPayloader.h
///

#ifndef QT6_QTRTPPAYLOADER_H_
#define QT6_QTRTPPAYLOADER_H_

#include <QByteArray>
#include <QObject>

#include "qt6/QtCommon.h"
#include "rtp/rtp.h"

namespace mediax::qt6 {

/// A RTP payloader base class
class QtRtpPayloader : public QObject {
  Q_OBJECT

 public:
  ///
  /// \brief Construct a new Qt Rtp Payloader object
  ///
  /// \param parent
  ///
  explicit QtRtpPayloader(QObject* parent = nullptr);

  ///
  /// \brief Set the Stream Info object
  ///
  /// \param stream_information
  ///
  virtual void setStreamInfo(const mediax::rtp::StreamInformation& stream_information) = 0;

  ///
  /// \brief Open the RTP stream
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE virtual bool open() = 0;

  ///
  /// \brief Start the RTP stream
  ///
  ///
  Q_INVOKABLE virtual void start() = 0;

  ///
  /// \brief Stop the RTP stream
  ///
  ///
  Q_INVOKABLE virtual void stop() = 0;

  ///
  /// \brief Close the RTP stream
  ///
  ///
  Q_INVOKABLE virtual void close() = 0;

  ///
  /// \brief Transmit a frame to the RTP stream
  ///
  /// \param frame The frame to transmit
  /// \param blocking Set to true if blocking
  ///
  Q_INVOKABLE virtual int transmit(Frame* frame, bool blocking = true) = 0;

 public slots:

  ///
  /// \brief A frame to transmit
  ///
  /// \param frame The frame to receive
  ///
  virtual void sendFrame(Frame frame) = 0;
};

}  // namespace mediax::qt6

#endif  // QT6_QTRTPPAYLOADER_H_
