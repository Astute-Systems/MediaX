
//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief RTP streaming video class for H.264 DEF-STAN 00-82 video streams
///
/// \file rtph264_depayloader.h
///

#ifndef H264_VAAPI_RTPH264_DEPAYLOADER_H_
#define H264_VAAPI_RTPH264_DEPAYLOADER_H_

#include <vector>

/// The MediaX Intel Video Accelleration API (VAAPI) namespace
namespace mediax::rtp::h264::vaapi {

class RtpH264VaapiDepayloader : public RtpDepayloader {
 public:
  ///
  /// \brief Construct a new Rtpvraw Depayloader object
  ///
  ///
  RtpH264VaapiDepayloader() = default;

  ///
  /// \brief Destroy the Rtpvraw Depayloader object
  ///
  ///
  ~RtpH264VaapiDepayloader() final;

  ///
  /// \brief Copy operator (Deleted)
  ///
  /// \param other
  /// \return RtpH264Depayloader
  ///
  RtpH264VaapiDepayloader &operator=(const RtpH264VaapiDepayloader &other) { return *this; }

  ///
  /// \brief Construct a new RtpH264Depayloader object (Deleted)
  ///
  /// \param other
  ///
  RtpH264VaapiDepayloader(RtpH264VaapiDepayloader &&other) = delete;

  ///
  /// \brief Configure at RTP input stream and dont wait for the SAP/SDP announcement
  ///
  /// \param stream_information set the stream information
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
  /// \brief Start the stream
  ///
  ///
  void Start() final;

  ///
  /// \brief Stop the stream, can be quickly re-started
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
  /// \return true when frame available
  /// \return false when no frame was received in the timeout
  ///
  bool Receive(uint8_t **cpu, int32_t timeout = 0) final;

  ///
  /// \brief Set new frame available
  ///
  ///
  void NewFrame();

 private:
  /// The GStreamer pipeline
  GstElement *pipeline_;
  /// A flag indication a new frame is available
  bool new_rx_frame_ = false;
};

}  // namespace mediax::rtp::h264::vaapi

#endif  // H264_GST_RTP_H264_DEPAYLOADER_H_

#endif  // H264_VAAPI_RTPH264_DEPAYLOADER_H_
