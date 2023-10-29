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
#include <map>

#include "sap/sap_announcer.h"

namespace mediax::qt {

class QtSapAnnouncer : public QObject {
  Q_OBJECT

 public:
  ///
  /// \brief Construct a new Qt Sap Announcer object
  ///
  /// \param parent
  ///
  explicit QtSapAnnouncer(QObject* parent = nullptr);

  ///
  /// \brief Destroy the Qt Sap Announcer object
  ///
  ///
  ~QtSapAnnouncer() final = default;

  ///
  /// \brief Get the SAP/SDP announcer singleton instance
  ///
  /// \return QtSapAnnouncer&
  ///
  static QtSapAnnouncer& getInstance();

  ///
  /// \brief Add a SAP announcement
  ///
  /// \param stream_information
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE void addSapAnnouncement(const ::mediax::rtp::StreamInformation& stream_information);

  ///
  /// \brief Get the Sap Announcement object
  ///
  /// \param session_name
  /// \return Q_INVOKABLE&
  ///
  Q_INVOKABLE ::mediax::rtp::StreamInformation& getSapAnnouncement(QString session_name);

  ///
  /// \brief Delete a SAP announcement
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE void deleteSapAnnouncement(QString stream_name);

  ///
  /// \brief Delete all SAP announcements
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE void deleteAllSapAnnouncements();

  ///
  /// \brief Start the SAP server
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE void start();

  ///
  /// \brief Stop the SAP server
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE void stop();

  ///
  /// \brief Restart all deleted SAP announcements
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE void restart();

  ///
  /// \brief Set the Source Interface object
  ///
  /// \param select
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE void setSourceInterface(uint16_t select = 0);

  ///
  /// \brief Get the interfaces as nic names
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE std::map<uint32_t, QString> GetInterfaces();

  ///
  /// \brief Get the Active Stream Count object
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE uint32_t getActiveStreamCount() const;

 signals:
  ///
  /// \brief A callback for a SAP message received
  ///
  /// \param count
  ///
  void activeStreamCountChanged(uint32_t count);

 private:
  /// The underlying SAP announcer
  ::mediax::sap::SapAnnouncer& sap_announcer_ = ::mediax::sap::SapAnnouncer::GetInstance();
};

}  // namespace mediax::qt

#endif  // QT_QTSAPANNOUNCER_H_
