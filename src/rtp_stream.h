//
// Copyright (C) 2023 DefenceX PTY LTD. All rights reserved.
//
// This software is distributed under the included copyright license.
// Any redistribution or reproduction, in part or in whole, in any form or medium, is strictly prohibited without the
// prior written consent of DefenceX PTY LTD.
//
// For any inquiries or concerns, please contact:
// DefenceX PTY LTD
// Email: enquiries@defencex.ai
//
///
/// \brief RTP streaming video class for uncompressed DEF-STAN 00-82 video streams
///
/// \file rtp_stream.h
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

#ifndef __RTP_STREAM_H__
#define __RTP_STREAM_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <string>
#include <thread>

#if _WIN32
#include <winsock2.h>
// Swap bytes in 16 bit value.
#define __bswap_constant_16(x) ((((x) >> 8) & 0xffu) | (((x)&0xffu) << 8))
// Swap bytes in 32 bit value.
#define __bswap_constant_32(x) \
  ((((x)&0xff000000u) >> 24) | (((x)&0x00ff0000u) >> 8) | (((x)&0x0000ff00u) << 8) | (((x)&0x000000ffu) << 24))
#define __bswap_32(x)                  \
  (__extension__({                     \
    register unsigned int __bsx = (x); \
    __bswap_constant_32(__bsx);        \
  }))
#define __bswap_16(x)               \
  (__extension__({                  \
    unsigned short int __bsx = (x); \
    __bswap_constant_16(__bsx);     \
  }))
#else
#include <byteswap.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif
#include <limits.h>

#include "colourspace.h"
#include "rtp_types.h"

//
// rtpstream RGB data
//
class RtpStream {
 public:
  /// The supported colour spaces
  enum class ColourspaceType { kColourspaceUndefined, kColourspaceRgb24, kColourspaceYuv, kColourspaceMono8 };

  ///
  /// \brief Construct a new Rtp Stream object
  ///
  /// \param height in pixels of the RTP stream
  /// \param width in pixels of the RTP stream
  ///
  RtpStream();

  ///
  /// \brief Destroy the Rtp Stream object
  ///
  ///
  ~RtpStream();

  ///
  /// \brief Configure an RTP output stream
  ///
  /// \param hostname IP address i.e. 239.192.1.1 for multicast
  /// \param port defaults to 5004
  ///
  void RtpStreamOut(std::string_view name, uint32_t height, uint32_t width, std::string_view hostname,
                    const uint16_t port = 5004);

  ///
  /// \brief Configure at RTP input stream
  ///
  /// \param hostname IP address i.e. 239.192.1.1 for multicast
  /// \param port defaults to 5004
  ///
  void RtpStreamIn(std::string_view name, ColourspaceType encoding, uint32_t height, uint32_t width,
                   std::string_view hostname, const uint16_t port = 5004);

  ///
  /// \brief Open the RTP stream
  ///
  /// \return true
  /// \return false
  ///
  bool Open();

  void Start();

  void Stop();

  ///
  /// \brief Close the RTP stream
  ///
  ///
  void Close() const;

  ///
  /// \brief Transmit an RGB buffer
  ///
  /// \param rgbframe pointer to the frame data
  /// \param blocking defaults to true, will wait till frame has been transmitted
  /// \return int
  ///
  int Transmit(uint8_t *rgbframe, bool blocking = true);

  ///
  /// \brief Recieve a frame or timeout
  ///
  /// \param cpu the fame buffer in CPU memory.
  /// \param timeout zero will wait forever or a timeout in milliseconds
  /// \return true when frame available
  /// \return false when no frame was received in the timeout
  ///
  bool Receive(uint8_t **cpu, int32_t timeout = 0);

 private:
  /// The incremental sequence numer for transmitting RTP packets
  static uint32_t sequence_number_;
  static bool new_rx_frame_;
  static bool rx_thread_running_;

  /// Height in pixels of stream
  uint32_t height_ = 0;

  /// Width in pixels of stream
  uint32_t width_ = 0;

  /// Intended update framerate
  uint32_t framerate_ = 25;

  /// The encoded video type
  ColourspaceType encoding_ = ColourspaceType::kColourspaceUndefined;

  /// Transmit arguments used by the thread
  TxData arg_tx;

  int sockfd_in_;
  std::string stream_in_name_ = "unknown";

  int sockfd_out_;
  std::string stream_out_name_ = "unknown";
  struct sockaddr_in server_addr_out_;
  socklen_t server_len_out_;
  pthread_mutex_t mutex_;
  std::array<uint8_t, kMaxUdpData> udpdata;
  std::vector<uint8_t> buffer_in_;
  struct addrinfo *server_out_;

  // Ingress port
  std::string hostname_in_;
  uint16_t port_no_in_ = 0;
  // Egress port
  std::string hostname_out_;
  uint16_t port_no_out_ = 0;

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
  static void TransmitThread(RtpStream *stream);

  ///
  /// \brief Recieve RTP data to the network using a separate thread
  ///
  /// \param stream this object
  ///
  static void ReceiveThread(RtpStream *stream);

  ///
  /// \brief Wait for a frame or timeout
  ///
  /// \param cpu
  /// \param timeout
  /// \return true
  /// \return false
  ///
  bool WaitForFrame(uint8_t **cpu, int32_t timeout);

  ///
  /// \brief Get a 90Htz timestamp
  ///
  /// \return int32_t The current time stamp
  ///
  static int32_t GenerateTimestamp90kHz();
  // Arguments sent to thread
  std::thread rx_thread_;
  std::thread tx_thread_;
};

#endif
