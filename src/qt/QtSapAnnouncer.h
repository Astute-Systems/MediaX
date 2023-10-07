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
/// \file QtSapAnnouncer.h
///

#ifndef QT_QTSAPANNOUNCER_H_
#define QT_QTSAPANNOUNCER_H_

#include <QObject>

#include "sap/sap_announcer.h"

namespace mediax::qt {

class QTSAPAnnouncer : public QObject {
  Q_OBJECT

 public:
  explicit QTSAPAnnouncer(QObject* parent = nullptr);

  Q_INVOKABLE void addSAPAnnouncement(const ::mediax::StreamInformation& stream_information);

  Q_INVOKABLE void deleteAllSAPAnnouncements();

  Q_INVOKABLE void start();

  Q_INVOKABLE void stop();

  Q_INVOKABLE void setSourceInterface(uint16_t select = 0);

  Q_INVOKABLE void listInterfaces(uint16_t select = 0);

  Q_INVOKABLE uint32_t getActiveStreamCount() const;

 signals:
  void activeStreamCountChanged(uint32_t count);

 private:
  ::mediax::sap::SAPAnnouncer& sap_announcer_;
};

}  // namespace mediax::qt

#endif  // QT_QTSAPANNOUNCER_H_
