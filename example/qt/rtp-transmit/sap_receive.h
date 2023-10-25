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

#ifndef SAP_RECEIVE_H
#define SAP_RECEIVE_H

#include <QObject>
#include <QRtp>
#include <QTimer>

#include "sap/sap.h"

class QtSapReceiver : public QObject {
  Q_OBJECT

 public:
  ///
  /// \brief Construct a new Qt SAP/SDP Receive object
  ///
  ///
  QtSapReceiver() = default;

  ///
  /// \brief Destroy the Qt SAP/SDP Receive object
  ///
  ///
  ~QtSapReceiver() = default;

 public slots:
  ///
  /// \brief New frame signal
  ///
  /// \param frame
  ///
  void newSap(const std::string name, const ::mediax::sap::SdpMessage message);

 private:
};

#endif  // SAP_RECEIVE_H
