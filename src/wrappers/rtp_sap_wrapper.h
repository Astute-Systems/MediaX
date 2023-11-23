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

#ifndef WRAPPERS_RTP_SAP_WRAPPER_H_
#define WRAPPERS_RTP_SAP_WRAPPER_H_

#include <chrono>
#include <string>

#include "rtp/rtp.h"
#include "sap/sap.h"
#include "utils/colourspace_cpu.h"

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
    if (!mediax::IsRtpInitialised()) mediax::InitRtp(0, nullptr);
    sap_announcer_.AddSapAnnouncement(stream_info_);
    sap_announcer_.SetSourceInterface();
    sap_announcer_.Start();
    rtp_payloader_.SetStreamInfo(stream_info_);
    rtp_payloader_.Open();
    rtp_payloader_.Start();
    last_transmit_timestamp_ = std::chrono::system_clock::now();
  }

  ///
  /// \brief Get the frame buffer, resized and ready to use
  ///
  /// \return vector<uint8_t>&
  ///
  std::vector<uint8_t>& GetBuffer() {
    data_buffer_.resize(stream_info_.height * stream_info_.width * (BitsPerPixel(stream_info_.encoding) / 8));
    return data_buffer_;
  }

  ///
  /// \brief Get the frame buffer containing a pre-defined test pattern
  ///
  /// List of test patterns supported
  /// + 0=EBU Colour Bars
  /// + 1=Colour Bars
  /// + 2=Grey Scale Bars
  /// + 3=Checkered
  /// + 4=Red
  /// + 5=Green
  /// + 6=Blue
  /// + 7=Black
  /// + 8=White
  /// + 9=White Noise
  ///
  /// \param pattern The test pattern generate from the list above, see rtp_utils.h
  /// \return std::vector<uint8_t>&
  ///
  std::vector<uint8_t>& GetBufferTestPattern(uint32_t pattern = 0) {
    std::vector<uint8_t>& buffer = GetBuffer();
    switch (pattern) {
      case 0:
        CreateColourBarEbuTestCard(buffer.data(), stream_info_.width, stream_info_.height, stream_info_.encoding);
        break;
      case 1:
        CreateColourBarTestCard(buffer.data(), stream_info_.width, stream_info_.height, stream_info_.encoding);
        break;
      case 2:
        CreateGreyScaleBarTestCard(buffer.data(), stream_info_.width, stream_info_.height, stream_info_.encoding);
        break;
      case 3:
        CreateCheckeredTestCard(buffer.data(), stream_info_.width, stream_info_.height, stream_info_.encoding);
        break;
      case 4:
        // red
        CreateSolidTestCard(buffer.data(), stream_info_.width, stream_info_.height, 0xff, 0xff, 0xff,
                            stream_info_.encoding);
        break;
      case 5:
        // green
        CreateSolidTestCard(buffer.data(), stream_info_.width, stream_info_.height, 0x00, 0xff, 0x00,
                            stream_info_.encoding);
        break;
      case 6:
        // blue
        CreateSolidTestCard(buffer.data(), stream_info_.width, stream_info_.height, 0x00, 0x00, 0xff,
                            stream_info_.encoding);
        break;
      case 7:
        // black
        CreateSolidTestCard(buffer.data(), stream_info_.width, stream_info_.height, 0x00, 0x00, 0x00,
                            stream_info_.encoding);
        break;
      case 8:
        // white
        CreateSolidTestCard(buffer.data(), stream_info_.width, stream_info_.height, 0xff, 0xff, 0xff,
                            stream_info_.encoding);
        break;
      case 9:
        CreateWhiteNoiseTestCard(buffer.data(), stream_info_.width, stream_info_.height, stream_info_.encoding);
        break;
      default:
        // black
        CreateSolidTestCard(buffer.data(), stream_info_.width, stream_info_.height, 0x00, 0x00, 0x00,
                            stream_info_.encoding);
        break;
    }
    return GetBuffer();
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
  /// The stream information
  ::mediax::rtp::StreamInformation stream_info_;
  /// The vectored data buffer
  std::vector<uint8_t> data_buffer_;
  /// Last transmit timestamp
  std::chrono::system_clock::time_point last_transmit_timestamp_;
};

}  // namespace mediax

#endif  // WRAPPERS_RTP_SAP_WRAPPER_H_
