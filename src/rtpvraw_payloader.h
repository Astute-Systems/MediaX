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
/// \file rtpvraw_payloader.h
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

#ifndef __RTPVRAW_PAYLOADER_H
#define __RTPVRAW_PAYLOADER_H

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
#if _WIN32
#include <winsock2.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif
#include "rtp_payloader.h"
#include "rtp_types.h"
#include "sap_listener.h"

///
/// \brief Manage an RTP stream
///
///
class RtpvrawPayloader : public RtpPayloader {
 public:
  /// The supported colour spaces

  ///
  /// \brief Construct a new Rtp Stream object
  ///
  /// \param height in pixels of the RTP stream
  /// \param width in pixels of the RTP stream
  ///
  RtpvrawPayloader();

  ///
  /// \brief Destroy the Rtp Stream object
  ///
  ///
  ~RtpvrawPayloader();

  ///
  /// \brief Configure an RTP output stream
  ///
  /// \param hostname IP address i.e. 239.192.1.1 for multicast
  /// \param port defaults to 5004
  /// \param name The name of the stream
  /// \param encoding The colour space of the stream
  /// \param height The height of the stream in pixels
  /// \param width The width of the stream in pixels
  ///
  void SetStreamInfo(std::string_view name, ColourspaceType encoding, uint32_t height, uint32_t width,
                     std::string_view hostname, const uint32_t port = 5004) final;

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
  /// \param stream
  /// \return int
  ///
  static void SendFrame(RtpvrawPayloader *stream);

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
  TxData arg_tx;
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
  /// \param last true if last line
  /// \param timestamp the timestamp
  /// \param source the source id
  ///
  void UpdateHeader(Header *packet, int line, int last, int32_t timestamp, int32_t source) const;

  ///
  /// \brief Transmit RTP data to the network using a separate thread
  /// \param stream this object
  /// \return void
  ///
  static void TransmitThread(RtpvrawPayloader *stream);

  ///
  /// \brief Get a 90Htz timestamp
  ///
  /// \return int32_t The current time stamp
  ///
  static int32_t GenerateTimestamp90kHz();
};

#endif  // __RTPVRAW_PAYLOADER_H
