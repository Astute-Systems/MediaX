//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video class for uncompressed DEF-STAN 00-82 video streams
///
/// \file rtp_uncompressed_payloader.h
///
///
/// Example RTP packet from wireshark
///      Real-Time Transport Protocol      10.. .... = Version: RFC 1889 Version (2)
///      ..0. .... = Padding: False
///      ...0 .... = Extension: False
///      .... 0000 = Contributing source identifiers count: 0
///      0... .... = Marker: False
///      Payload type: DynamicRTP-Type-96 (96)
///      Sequence number: 34513
///      Timestamp: 2999318601
///      Synchronization Source identifier: 0xdccae7a8 (3704285096)
///      Payload: 000003c000a08000019e00a2000029292929f06e29292929...
///
///
/// Gstreamer1.0 working example UYVY streaming
/// ===========================================
/// gst-launch-1.0 videotestsrc num_buffers ! video/x-raw, format=UYVY, framerate=25/1, width=640, height=480 ! queue !
/// rtpvrawpay ! udpsink host=127.0.0.1 port=5004
///
/// gst-launch-1.0 udpsrc port=5004 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000,
/// encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)480, height=(string)480,
/// payload=(int)96" ! queue ! rtpvrawdepay ! queue ! xvimagesink sync=false

/// Use his program to stream data to the udpsrc example above on the tegra X1

#ifndef UNCOMPRESSED_RTP_UNCOMPRESSED_PAYLOADER_H_
#define UNCOMPRESSED_RTP_UNCOMPRESSED_PAYLOADER_H_

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#if _WIN32
#include <winsock2.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif
#include "rtp/rtp_payloader.h"
#include "rtp/rtp_types.h"

namespace mediax::rtp::uncompressed {

///
/// \brief Manage an RTP stream
///
///
class RtpUncompressedPayloader : public ::mediax::rtp::RtpPayloader {
 public:
  /// The supported colour spaces

  ///
  /// \brief Construct a new Rtp Stream object
  ///
  /// \param height in pixels of the RTP stream
  /// \param width in pixels of the RTP stream
  ///
  RtpUncompressedPayloader();

  ///
  /// \brief Destroy the Rtp Stream object
  ///
  ///
  ~RtpUncompressedPayloader() final;

  ///
  /// \brief Configure an RTP output stream
  ///
  /// \param stream_information The stream information
  ///
  void SetStreamInfo(const ::mediax::rtp::StreamInformation &stream_information) final;

  ///
  /// \brief Open the RTP stream
  ///
  /// \return true
  /// \return false
  ///
  bool Open() final;

  ///
  /// \brief Close the RTP stream
  ///
  ///
  void Close() final;

  ///
  /// \brief Transmit an RGB buffer
  ///
  /// \param rgbframe pointer to the frame data
  /// \param blocking defaults to true, will wait till frame has been transmitted
  /// \return int
  ///
  int Transmit(uint8_t *rgbframe, bool blocking = true) final;

  ///
  /// \brief Send a frame
  ///
  /// \param stream The RtpUncompressedPayloader object
  ///
  static void SendFrame(RtpUncompressedPayloader *stream);

  ///
  /// \brief Get the Mutex object
  ///
  /// \return std::mutex&
  ///
  std::mutex &GetMutex() { return mutex_; }

 private:
  /// The incremental sequence numer for transmitting RTP packets
  static uint32_t sequence_number_;
  /// Transmit arguments used by the thread
  rtp::TxData arg_tx;
  /// The server address information
  struct addrinfo *server_out_;
  /// The socket for the outgoing stream
  struct sockaddr_in server_addr_out_;
  /// The length of the server address
  socklen_t server_len_out_;
  /// The buffer for the incoming RTP data
  static std::vector<uint8_t> buffer_in_;
  /// Arguments sent to thread
  std::thread tx_thread_;
  /// The mutex for the transmit thread
  std::mutex mutex_;

  ///
  /// \brief Populate the RTP header
  ///
  /// \param packet the RTP header
  /// \param line the line number
  /// \param bytes_per_pixel the bytes per pixel
  /// \param last true if last line
  /// \param timestamp the timestamp
  /// \param source the source id
  ///
  void UpdateHeader(rtp::RtpHeader *packet, int line, int bytes_per_pixel, int last, int32_t timestamp, int32_t source);

  ///
  /// \brief Transmit RTP data to the network using a separate thread
  ///
  /// \param stream The RtpUncompressedPayloader object
  ///
  static void TransmitThread(RtpUncompressedPayloader *stream);

  ///
  /// \brief Get a 90Htz timestamp
  ///
  /// \return int32_t The current time stamp
  ///
  static int32_t GenerateTimestamp90kHz();
};

}  // namespace mediax::rtp::uncompressed

#endif  // UNCOMPRESSED_RTP_UNCOMPRESSED_PAYLOADER_H_
