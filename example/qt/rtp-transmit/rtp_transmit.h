//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \file rtp_receive.h
///

#ifndef QT_RECEIVE_H
#define QT_RECEIVE_H
/// [QtTransmit example header]
#include <QObject>
#include <QRtp>
#include <QTimer>

class QtTransmit : public QObject {
  Q_OBJECT

 public:
  ///
  /// \brief Construct a new Qt Receive object
  ///
  ///
  QtTransmit();

  ///
  /// \brief Destroy the Qt Receive object
  ///
  ///
  ~QtTransmit();

 signals:
  ///
  /// \brief New frame signal
  ///
  /// \param frame
  ///
  void newFrame(QByteArray *frame);

 public slots:
  ///
  /// \brief Send a single frame to the payloader
  ///
  ///
  void sendFrame();

 private:
  /// The Qt RTP payloader
  mediax::qt::QtRtpUncompressedPayloader rtp;
  /// Frame counter
  int frame_count = 0;
};
/// [QtTransmit example header]

#endif  // QT_RECEIVE_H
