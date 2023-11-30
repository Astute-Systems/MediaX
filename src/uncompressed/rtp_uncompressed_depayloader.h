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
/// \file rtp_uncompressed_depayloader.h
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
/// \code
/// gst-launch-1.0 udpsrc port=5004 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000,
/// encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)480, height=(string)480,
/// payload=(int)96" ! queue ! rtpvrawdepay ! queue ! xvimagesink sync=false
/// \endcode
/// Use his program to stream data to the udpsrc example above on the tegra X1

#ifndef UNCOMPRESSED_RTP_UNCOMPRESSED_DEPAYLOADER_H_
#define UNCOMPRESSED_RTP_UNCOMPRESSED_DEPAYLOADER_H_

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
#include <atomic>
#include <limits>
#include <vector>

#include "rtp/rtp_depayloader.h"
#include "rtp/rtp_types.h"

/// The DefenceX media streaming namespace
namespace mediax::rtp::uncompressed {

///
/// \brief Manage an RTP stream
///
///
class RtpUncompressedDepayloader : public ::mediax::rtp::RtpDepayloader {
 public:
  /// The supported colour spaces

  ///
  /// \brief Construct a new Rtp Stream object
  ///
  /// \param height in pixels of the RTP stream
  /// \param width in pixels of the RTP stream
  ///
  RtpUncompressedDepayloader();

  ///
  /// \brief Destroy the Rtp Stream object
  ///
  ///
  ~RtpUncompressedDepayloader(void) final = default;

  ///
  /// \brief Construct a new Rtpvraw Depayloader object (Deleted)
  ///
  /// \param other
  ///
  RtpUncompressedDepayloader(RtpUncompressedDepayloader &&other) = delete;

  ///
  /// \brief Configure at RTP input stream and dont wait for the SAP/SDP announcement
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
  /// \brief Start the stream recieve thread, can be quickly re-started without having to re-open the UDP port
  ///
  /// This can cause bandwidth issues if multiple UDP multicast streams are open. If bandwidth is an issue then its
  /// better to close the stream before switching to a new stream
  ///
  void Start() final;

  ///
  /// \brief Stop the stream recieve thread, can be quickly re-started without having to re-open the UDP port
  ///
  ///
  void Stop() final;

  ///
  /// \brief Close the RTP stream
  ///
  ///
  void Close() final;

  ///
  /// \brief Recieve a frame or timeout
  ///
  /// \param cpu the fame buffer in CPU memory.
  /// \param timeout zero will wait forever or a timeout in milliseconds
  /// \return true when frame available and false when no frame was received in the timeout
  ///
  bool Receive(::mediax::rtp::RtpFrameData *data, int32_t timeout = 0) final;

  ///
  /// \brief The callback function for the RTP stream
  ///
  /// \param frame
  ///
  void Callback(::mediax::rtp::RtpFrameData frame) const final;

 private:
  /// The incremental sequence numer for transmitting RTP packets, atomic
  std::atomic<bool> new_rx_frame_ = false;
  /// Flag indicating the thread is running
  bool rx_thread_running_ = true;
  /// Transmit arguments used by the thread
  ::mediax::rtp::TxData arg_tx;
  /// thread mutex
  pthread_mutex_t mutex_;
  /// UDP data buffer
  std::array<uint8_t, ::mediax::rtp::kMaxUdpData> udpdata;
  /// Arguments sent to thread
  std::thread rx_thread_;
  // The current sequence number
  uint32_t sequence_number_ = 0;

  ///
  /// \brief Populate the RTP header
  ///
  /// \param packet the RTP header
  /// \param line the line number
  /// \param last true if last line
  /// \param timestamp the timestamp
  /// \param source the source id
  ///
  void UpdateHeader(::mediax::rtp::RtpHeaderData *packet, int line, int last, int32_t timestamp, int32_t source) const;

  ///
  /// \brief Transmit RTP data to the network using a separate thread
  /// \param stream this object
  /// \return void
  ///
  static void TransmitThread(RtpUncompressedDepayloader *stream);

  ///
  /// \brief Recieve RTP data to the network using a separate thread
  ///
  /// \param stream this object
  ///
  static void ReceiveThread(RtpUncompressedDepayloader *stream);

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

  ///
  /// \brief Read in a RTP packet and decode header
  ///
  /// \return true
  /// \return false
  ///
  bool ReadRtpHeader(RtpUncompressedDepayloader *stream, ::mediax::rtp::RtpPacket *packet);

  ///
  /// \brief Receive video lines
  ///
  /// \return true if frame complete
  /// \return false if frame incomplete
  ///
  bool ReceiveLines(::mediax::rtp::RtpPacket *packet, bool *last_scan_line, int *last_packet);
};

}  // namespace mediax::rtp::uncompressed

#endif  // UNCOMPRESSED_RTP_UNCOMPRESSED_DEPAYLOADER_H_
