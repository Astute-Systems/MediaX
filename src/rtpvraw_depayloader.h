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
/// \file rtpvraw_depayloader.h
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

#ifndef __RTPVRAW_DEPAYLOADER_H
#define __RTPVRAW_DEPAYLOADER_H

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
#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif
#include <limits.h>

#include "rtp_types.h"
#include "sap_listener.h"

///
/// \brief Manage an RTP stream
///
///
class RtpvrawDepayloader {
 public:
  /// The supported colour spaces

  ///
  /// \brief Construct a new Rtp Stream object
  ///
  /// \param height in pixels of the RTP stream
  /// \param width in pixels of the RTP stream
  ///
  RtpvrawDepayloader();

  ///
  /// \brief Destroy the Rtp Stream object
  ///
  ///
  ~RtpvrawDepayloader();

  ///
  /// \brief Configure at RTP input stream and dont wait for the SAP/SDP announcement
  ///
  /// \param hostname IP address i.e. 239.192.1.1 for multicast
  /// \param port defaults to 5004
  /// \param name The name of the stream
  /// \param encoding The encoding of the stream
  /// \param height The height of the stream in pixels
  /// \param width The width of the stream in pixels
  ///
  static void SetStreamInfo(std::string_view name, ColourspaceType encoding, uint32_t height, uint32_t width,
                            std::string_view hostname, const uint32_t port = 5004);

  ///
  /// \brief Configure at RTP input stream and  wait for the SAP/SDP announcement
  ///
  ///
  void SetStreamInfo(std::string_view name) const;

  ///
  /// \brief Register a SAP callback to get updated
  ///
  /// \param sdp
  ///
  static void SapCallback(const sap::SDPMessage &sdp);

  ///
  /// \brief Open the RTP stream
  ///
  /// \return true
  /// \return false
  ///
  bool Open() const;

  ///
  /// \brief Start the stream
  ///
  ///
  void Start();

  ///
  /// \brief Stop the stream, can be quickly re-started
  ///
  ///
  void Stop();

  ///
  /// \brief Close the RTP stream
  ///
  ///
  void Close() const;

  ///
  /// \brief Recieve a frame or timeout
  ///
  /// \param cpu the fame buffer in CPU memory.
  /// \param timeout zero will wait forever or a timeout in milliseconds
  /// \return true when frame available
  /// \return false when no frame was received in the timeout
  ///
  bool Receive(uint8_t **cpu, int32_t timeout = 0) const;

  ///
  /// \brief Get the Colour Space object of the incoming stream. \note This may be invalid id no SAP/SDP announcement
  /// has been received yet.
  ///
  /// \return ColourspaceType
  ///
  ColourspaceType GetColourSpace() const;

  ///
  /// \brief Get the Height object of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return uint32_t
  ///
  uint32_t GetHeight() const;

  ///
  /// \brief Get the Width object of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return uint32_t
  ///
  uint32_t GetWidth() const;

  ///
  /// \brief Get the Frame Rate of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return uint32_t
  ///
  uint32_t GetFrameRate() const;

  ///
  /// \brief Get the Ip Address of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return std::string
  ///
  std::string GetIpAddress() const;

  ///
  /// \brief Get the Port of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return uint32_t
  ///
  uint32_t GetPort() const;

 private:
  /// The incremental sequence numer for transmitting RTP packets
  static bool new_rx_frame_;
  static bool rx_thread_running_;

  /// The encoded video type
  ColourspaceType encoding_ = ColourspaceType::kColourspaceUndefined;

  /// Transmit arguments used by the thread
  TxData arg_tx;

  // Ingress port
  static PortType ingress_;

  pthread_mutex_t mutex_;
  std::array<uint8_t, kMaxUdpData> udpdata;
  static std::vector<uint8_t> buffer_in_;
  // Arguments sent to thread
  std::thread rx_thread_;

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
  static void TransmitThread(RtpvrawDepayloader *stream);

  ///
  /// \brief Recieve RTP data to the network using a separate thread
  ///
  /// \param stream this object
  ///
  static void ReceiveThread(RtpvrawDepayloader *stream);

  ///
  /// \brief Wait for a frame or timeout
  ///
  /// \param cpu
  /// \param timeout
  /// \return true
  /// \return false
  ///
  bool WaitForFrame(uint8_t **cpu, int32_t timeout) const;

  ///
  /// \brief Get a 90Htz timestamp
  ///
  /// \return int32_t The current time stamp
  ///
  static int32_t GenerateTimestamp90kHz();

  ///
  /// \brief Read in a RTP packet and decode header
  ///
  /// \return true
  /// \return false
  ///
  bool ReadRtpHeader(RtpvrawDepayloader *stream, RtpPacket *packet) const;
};

#endif  // __RTPVRAW_DEPAYLOADER_H
