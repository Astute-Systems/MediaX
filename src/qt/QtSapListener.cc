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

QTSAPListener::QTSAPListener(QObject *parent) : QObject(parent) {}

Q_INVOKABLE void QTSAPListener::start() { sap_listener_.Start(); }

Q_INVOKABLE void QTSAPListener::stop() { sap_listener_.Stop(); }

}  // namespace mediax::qt
