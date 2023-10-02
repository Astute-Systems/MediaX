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

#include "rtp/raw/rtp_depayloader.h"

namespace mediax::qt {

class RtpDepayloaderWrapper : public QObject {
  Q_OBJECT

 public:
  explicit RtpDepayloaderWrapper(QObject* parent = nullptr) : QObject(parent) {
    m_rtpDepayloader = std::make_unique<mediax::RtpDepayloader>();
  }

 public slots:
  void setStreamInfo(const mediax::StreamInformation& stream_information) {
    m_rtpDepayloader->SetStreamInfo(stream_information);
  }

  bool receive(uint8_t** cpu, int32_t timeout = 0) { return m_rtpDepayloader->Receive(cpu, timeout); }

 private:
  std::unique_ptr<mediax::RtpDepayloader> m_rtpDepayloader;
};

}  // namespace mediax::qt

#endif  // QT_QTRTPUNCOMPRESSEDDEPAYLOADER_H_
