//
// MIT License
//
// Copyright (c) 2023 Ross Newman (ross.newman@defencex.ai)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the 'Software'), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
// THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

/// Use his program to stream data to the udpsc example above on the tegra X1

#ifndef __RTP_STREAM_H__
#define __RTP_STREAM_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <array>
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

#include "rtp_types.h"

/// Helper functions for different colour space options
void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba);
void RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv);
void RgbaToYuvBasic(uint32_t width, uint32_t height, uint8_t *rgb_buffer, uint8_t *yuv_buffer);
void YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba);
void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb);

//
// rtpstream RGB data
//
class RtpStream {
 public:
  ///
  /// \brief Construct a new Rtp Stream object
  ///
  /// \param height in pixels of the RTP stream
  /// \param width in pixels of the RTP stream
  ///
  RtpStream(uint32_t height, uint32_t width);

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
  void RtpStreamOut(std::string_view hostname, const int port = 5004);

  ///
  /// \brief Configure at RTP input stream
  ///
  /// \param hostname IP address i.e. 239.192.1.1 for multicast
  /// \param port defaults to 5004
  ///
  void RtpStreamIn(std::string_view hostname, const int port = 5004);

  ///
  /// \brief Open the RTP stream
  ///
  /// \return true
  /// \return false
  ///
  bool Open();

  ///
  /// \brief Close the RTP stream
  ///
  ///
  void Close();

  ///
  /// \brief Transmit an RGB buffer
  ///
  /// \param rgbframe pointer to the frame data
  /// \return int
  ///
  int Transmit(uint8_t *rgbframe);

  ///
  /// \brief Recieve a frame or timeout
  ///
  /// \param cpu the fame buffer in CPU memory.
  /// \param timeout zero will wait forever or a timeout in milliseconds
  /// \return true
  /// \return false
  ///
  bool Receive(uint8_t **cpu, uint32_t timeout = 0);

  /// The incremental sequence numer for transmitting RTP packets
  static uint32_t sequence_number_;

  /// Height in pixels of stream
  uint32_t height_;

  /// Width in pixels of stream
  uint32_t width_;

  /// Transmit arguments used by the thread
  TxData arg_tx;

 private:
  int sockfd_in_;
  int sockfd_out_;
  struct sockaddr_in server_addr_in_;
  struct sockaddr_in server_addr_out_;
  socklen_t server_len_in_;
  socklen_t server_len_out_;
  pthread_mutex_t mutex_;
  unsigned int frame_ = 0;
  uint8_t *gpuBuffer;
  std::array<uint8_t, kMaxUdpData> udpdata;
  uint8_t *buffer_in_;
  struct hostent *server_in_;
  struct hostent *server_out_;

  // Ingress port
  std::string hostname_in_;
  int8_t port_no_in_ = 0;
  // Egress port
  std::string hostname_out_;
  int8_t port_no_out_ = 0;

  ///
  /// \brief Populate the RTP header
  ///
  /// \param packet the RTP header
  /// \param line the line number
  /// \param last true if last line
  /// \param timestamp the timestamp
  /// \param source the source id
  ///
  void UpdateHeader(Header *packet, int line, int last, int32_t timestamp, int32_t source);

  ///
  /// \brief Transmit RTP data to the network using a separate thread
  /// \param stream this object
  /// \return void
  ///
  void TransmitThread(RtpStream *stream);

  ///
  /// \brief Recieve RTP data to the network using a separate thread
  ///
  /// \param stream this object
  ///
  void ReceiveThread(RtpStream *stream);

  // Arguments sent to thread
  sched_param param;
  std::thread rx_thread_;
  std::thread tx_thread_;
};

#endif
