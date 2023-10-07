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
/// \file QtSapAnnouncer.cc
///

#include "qt/QtSapAnnouncer.h"

namespace mediax::qt {

QTSAPAnnouncer::QTSAPAnnouncer(QObject *parent)
    : QObject(parent), sap_announcer_(::mediax::sap::SAPAnnouncer::GetInstance()) {}

Q_INVOKABLE void QTSAPAnnouncer::addSAPAnnouncement(const ::mediax::StreamInformation &stream_information) {
  sap_announcer_.AddSAPAnnouncement(stream_information);
}

Q_INVOKABLE void QTSAPAnnouncer::deleteAllSAPAnnouncements() { sap_announcer_.DeleteAllSAPAnnouncements(); }

Q_INVOKABLE void QTSAPAnnouncer::start() { sap_announcer_.Start(); }

Q_INVOKABLE void QTSAPAnnouncer::stop() { sap_announcer_.Stop(); }

Q_INVOKABLE void QTSAPAnnouncer::setSourceInterface(uint16_t select) { sap_announcer_.SetSourceInterface(select); }

Q_INVOKABLE void QTSAPAnnouncer::listInterfaces(uint16_t select) { sap_announcer_.ListInterfaces(select); }

Q_INVOKABLE uint32_t QTSAPAnnouncer::getActiveStreamCount() const { return sap_announcer_.GetActiveStreamCount(); }

}  // namespace mediax::qt
