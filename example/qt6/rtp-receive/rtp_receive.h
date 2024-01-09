//
// Copyright (c) 2024, DefenceX PTY LTD
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
/// [QtReceive example header]
#include <QObject>
#include <QRtp>
#include <QTimer>

class QtReceive : public QObject {
  Q_OBJECT

 public:
  ///
  /// \brief Construct a new Qt Receive object
  ///
  ///
  QtReceive();

  ///
  /// \brief Destroy the Qt Receive object
  ///
  ///
  ~QtReceive();

 public slots:
  ///
  /// \brief Send a single frame to the payloader
  ///
  ///
  void newFrame(Frame frame);

 private:
  /// The Qt RTP payloader
  mediax::qt6::QtRtpUncompressedDepayloader rtp;
  /// Frame counter
  int frame_count = 0;
};
/// [QtReceive example header]

#endif  // QT_RECEIVE_H
