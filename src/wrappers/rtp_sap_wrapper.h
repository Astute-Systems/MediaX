//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video types
///
/// \file rtp_sap_wrapper.h

#ifndef RTP_SAP_WRAPPER_H
#define RTP_SAP_WRAPPER_H

#include <chrono>
#include <string>

#include "rtp/rtp.h"
#include "sap/sap.h"

namespace mediax {

template <typename T>
class RtpSapTransmit {
 public:
  ///
  /// \brief Construct a new Rtp Sap Transmit object
  ///
  /// \param hostname The IPV4 multicast address
  /// \param port The IPV4 multicast port
  /// \param name The name of the stream
  /// \param height The height of the stream
  /// \param width The width of the stream
  /// \param framerate The framerate of the stream
  /// \param encoding The encoding of the stream
  ///
  RtpSapTransmit(std::string hostname, uint16_t port, std::string session_name, uint16_t height, uint16_t width,
                 uint16_t framerate, std::string encoding) {
    stream_info_ = {.session_name = session_name,
                    .hostname = hostname,
                    .port = port,
                    .height = height,
                    .width = width,
                    .framerate = framerate,
                    .encoding = ::mediax::ColourspaceTypeFromString(encoding)};
    mediax::InitRtp(0, nullptr);
    sap_announcer_.AddSapAnnouncement(stream_info_);
    sap_announcer_.Start();
    rtp_payloader_.SetStreamInfo(stream_info_);
    rtp_payloader_.Open();
    rtp_payloader_.Start();
    last_transmit_timestamp_ = std::chrono::system_clock::now();
  }

  ///
  /// \brief Destroy the Rtp Sap Transmit object
  ///
  ///
  ~RtpSapTransmit() {
    sap_announcer_.Stop();
    rtp_payloader_.Stop();
    rtp_payloader_.Close();
    ::mediax::RtpCleanup();
  }

  ///
  /// \brief The frame transmit function, this will transmit the frame over RTP at the required frequency
  ///
  /// \param data The RGB frame data
  /// \param size The size of the RGB frame data
  ///
  void Transmit(uint8_t* data, size_t size) {
    // Check if we need to transmit yet
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() -
                                                                         last_transmit_timestamp_)
                       .count();
    int interval = 1000 / stream_info_.framerate;
    if (elapsed < 1000 / stream_info_.framerate) {
      // Delay the transmit by the elapsed time minus the required delay
      std::this_thread::sleep_for(std::chrono::milliseconds(interval - elapsed));
    }
    rtp_payloader_.Transmit(data, size);
    // update timestamp
    last_transmit_timestamp_ = std::chrono::system_clock::now();
  }

 private:
  /// The RTP payloader
  T rtp_payloader_;
  /// The SAP announcer
  sap::SapAnnouncer& sap_announcer_ = ::mediax::sap::SapAnnouncer::GetInstance();
  ::mediax::rtp::StreamInformation stream_info_;
  /// Last transmit timestamp
  std::chrono::system_clock::time_point last_transmit_timestamp_;
};

}  // namespace mediax

#endif  // RTP_SAP_WRAPPER_H
