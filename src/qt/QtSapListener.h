//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief Session Announcement Protocol (SDP) implementation for announcement of the stream data. The SAP packet
/// contains the Session Description Protocol (SDP).
///
/// \file QtSapListener.h
///

#ifndef QT_QTSAPLISTENER_H_
#define QT_QTSAPLISTENER_H_

#include <QObject>
#include <string>

#include "sap/sap_listener.h"

/// The Qt wrappers namespace
namespace mediax::qt {

class QTSAPListener : public QObject {
  Q_OBJECT

 public:
  explicit QTSAPListener(QObject *parent = nullptr);

  Q_INVOKABLE void start();

  Q_INVOKABLE void stop();

 signals:
  void sapDataReceived(const std::string &name, const ::mediax::sap::SDPMessage &message);

 private:
  ::mediax::sap::SAPListener sap_listener_;
};

}  // namespace mediax::qt

#endif  // QT_QTSAPLISTENER_H_
