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
/// \file QtSapListener.cc
///

#include "qt/QtSapListener.h"

namespace mediax::qt {

void QtSapListener::SapCallback(sap::SDPMessage *sdp) {
  std::string name = "unknown";
  emit ::mediax::qt::QtSapListener::GetInstance().sapDataReceived(name, *sdp);
}

QtSapListener &QtSapListener::GetInstance() {
  static std::unique_ptr<QtSapListener> m_instance = std::make_unique<QtSapListener>();
  return *m_instance;
}

QtSapListener::QtSapListener(QObject *parent) : QObject(parent) { sap_listener_.RegisterSapListener("", SapCallback); }

Q_INVOKABLE void QtSapListener::start() { sap_listener_.Start(); }

Q_INVOKABLE void QtSapListener::stop() { sap_listener_.Stop(); }

}  // namespace mediax::qt
