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
/// \file QtSapListener.cc
///

#include "qt6/QtSapListener.h"

#include <iostream>

namespace mediax::qt6 {

void QtSapListener::SapCallback(const sap::SdpMessage *sdp, void *data) {
  auto *listener = reinterpret_cast<QtSapListener *>(data);
  // emit a signal here in the future
  emit listener->sapData(QString::fromStdString(sdp->session_name), *sdp);
}

QtSapListener &QtSapListener::getInstance() {
  static auto m_instance = std::make_shared<QtSapListener>();
  return *m_instance;
}

QtSapListener::QtSapListener(QObject *parent) : QObject(parent) {
  sap_listener_.RegisterSapListener("", SapCallback, reinterpret_cast<void *>(this));
}

Q_INVOKABLE void QtSapListener::start() { sap_listener_.Start(); }

Q_INVOKABLE void QtSapListener::stop() { sap_listener_.Stop(); }

}  // namespace mediax::qt6
