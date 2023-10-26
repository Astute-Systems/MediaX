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
/// \file QtRtpUncompressedDepayloader.h
///

#ifndef QT_QTRTPUNCOMPRESSEDDEPAYLOADER_H_
#define QT_QTRTPUNCOMPRESSEDDEPAYLOADER_H_

#include <QObject>
#include <memory>

#include "uncompressed/rtp_uncompressed_depayloader.h"

namespace mediax::qt {

class QtRtpUncompressedDepayloader : public QObject {
  Q_OBJECT

 public:
  ///
  /// \brief Construct a new Rtp Uncompressed Depayloader object
  ///
  /// \param parent
  ///
  explicit QtRtpUncompressedDepayloader(QObject* parent = nullptr) : QObject(parent) {
    m_depayloader = std::make_unique<mediax::rtp::uncompressed::RtpUncompressedDepayloader>();
  }

 public slots:
  ///
  /// \brief Set the Stream Info object
  ///
  /// \param stream_information
  ///
  void setStreamInfo(const mediax::rtp::StreamInformation& stream_information) {
    m_depayloader->SetStreamInfo(stream_information);
  }

  ///
  /// \brief Receive a frame from the RTP stream
  ///
  /// \param cpu
  /// \param timeout
  /// \return true
  /// \return false
  ///
  bool receive(uint8_t** cpu, int32_t timeout = 0);

  ///
  /// \brief Get the State object
  ///
  /// \return Q_INVOKABLE
  ///
  Q_INVOKABLE mediax::rtp::StreamState getState() const;

 private:
  /// The underlying RTP depayloader
  std::unique_ptr<mediax::rtp::uncompressed::RtpUncompressedDepayloader> m_depayloader;
};

}  // namespace mediax::qt

#endif  // QT_QTRTPUNCOMPRESSEDDEPAYLOADER_H_
