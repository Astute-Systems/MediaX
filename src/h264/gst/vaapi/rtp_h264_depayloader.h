
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

#ifndef H264_GST_NVIDIA_RTP_H264_DEPAYLOADER_H_
#define H264_GST_NVIDIA_RTP_H264_DEPAYLOADER_H_

#include <gst/app/gstappsink.h>
#include <gst/gst.h>

#include <vector>

#include "rtp/rtp_depayloader.h"

/// The Gstreamer implementation of the Nvidia (NVENC) namespace
namespace mediax::h264::gst::vaapi {

GstFlowReturn NewFrameCallback(GstAppSink *appsink, gpointer user_data);

class RtpH264Depayloader : public RtpDepayloader {
 public:
  ///
  /// \brief Construct a new Rtpvraw Depayloader object
  ///
  ///
  RtpH264Depayloader() = default;

  ///
  /// \brief Destroy the Rtpvraw Depayloader object
  ///
  ///
  ~RtpH264Depayloader() final;

  ///
  /// \brief Copy operator (Deleted)
  ///
  /// \param other
  /// \return RtpH264Depayloader
  ///
  RtpH264Depayloader &operator=(const RtpH264Depayloader &other) { return *this; }

  ///
  /// \brief Construct a new RtpH264Depayloader object (Deleted)
  ///
  /// \param other
  ///
  RtpH264Depayloader(RtpH264Depayloader &&other) = delete;

  ///
  /// \brief Configure at RTP input stream and dont wait for the SAP/SDP announcement
  ///
  /// \param stream_information set the stream information
  ///
  void SetStreamInfo(const ::mediax::StreamInformation &stream_information) final;

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
  /// \brief Get the Buffer object
  ///
  /// \return uint8_t*
  ///
  std::vector<uint8_t> &GetBuffer();

  ///
  /// \brief Set new frame available
  ///
  ///
  void NewFrame();

 private:
  /// The GStreamer pipeline
  GstElement *pipeline_;
  /// The video buffer
  std::vector<uint8_t> buffer_in_;
  /// A flag indication a new frame is available
  bool new_rx_frame_ = false;
};

}  // namespace mediax::h264::gst::vaapi

#endif  // H264_GST_NVIDIA_RTP_H264_DEPAYLOADER_H_
