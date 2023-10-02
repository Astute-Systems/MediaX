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
/// \file QtRtpUncompressedPayloader.h
///

#ifndef QT_QTRTPUNCOMPRESSEDPAYLOADER_H_
#define QT_QTRTPUNCOMPRESSEDPAYLOADER_H_

#include <QObject>
#include <memory>

#include "uncompressed/rtp_uncompressed_payloader.h"

namespace mediax::qt {

class RtpPayloaderWrapper : public QObject {
  Q_OBJECT

 public:
  explicit RtpPayloaderWrapper(QObject* parent = nullptr) : QObject(parent) {
    m_rtpPayloader = std::make_unique<mediax::RtpUncompressedPayloader>();
  }

 public slots:
  void setStreamInfo(const mediax::StreamInformation& stream_information) {
    m_rtpPayloader->SetStreamInfo(stream_information);
  }

  int transmit(uint8_t* rgbframe, bool blocking = true) { return m_rtpPayloader->Transmit(rgbframe, blocking); }

 private:
  std::unique_ptr<mediax::RtpPayloader> m_rtpPayloader;
};

}  // namespace mediax::qt

#endif  // QT_QTRTPUNCOMPRESSEDPAYLOADER_H_
