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

namespace mediax::qt {

class QTSAPListener : public QObject {
  Q_OBJECT

 public:
  explicit QTSAPListener(QObject *parent = nullptr) : QObject(parent) {}

  Q_INVOKABLE void addSapCallback(const std::string &name, const SapCallback &callback) {
    sap_listener_.AddSapCallback(name, callback);
  }

  Q_INVOKABLE void removeSapCallback(const std::string &name) { sap_listener_.RemoveSapCallback(name); }

  Q_INVOKABLE void clearSapCallbacks() { sap_listener_.ClearSapCallbacks(); }

  Q_INVOKABLE void start() { sap_listener_.Start(); }

  Q_INVOKABLE void stop() { sap_listener_.Stop(); }

  Q_INVOKABLE void setMulticastAddress(const std::string &address) { sap_listener_.SetMulticastAddress(address); }

  Q_INVOKABLE void setMulticastPort(uint16_t port) { sap_listener_.SetMulticastPort(port); }

 signals:
  void sapDataReceived(const std::string &name, const SDPMessage &message);

 private:
  ::mediax::sap::SAPListener sap_listener_;
};

}  // namespace mediax::qt

#endif  // QT_QTSAPLISTENER_H_
