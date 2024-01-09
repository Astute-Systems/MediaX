
//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief RTP streaming video class for H.264 DEF-STAN 00-82 video streams
///
/// \file rtp_h264_depayloader.h
///

#ifndef H264_GST_NVIDIA_RTP_H264_DEPAYLOADER_H_
#define H264_GST_NVIDIA_RTP_H264_DEPAYLOADER_H_

#include <gst/app/gstappsink.h>
#include <gst/gst.h>

#include <vector>

#include "rtp/rtp_depayloader.h"

/// The Gstreamer implementation of the Intel Video Accelleration API (VAAPI) namespace
namespace mediax::rtp::h264::gst::nvidia {

/// A RTP payloader for H.264 DEF-STAN 00-82 video streams
class RtpH264GstNvidiaDepayloader : public mediax::rtp::RtpDepayloader {
 public:
  ///
  /// \brief Construct a new Rtpvraw Depayloader object
  ///
  ///
  RtpH264GstNvidiaDepayloader();

  ///
  /// \brief Destroy the Rtpvraw Depayloader object
  ///
  ///
  ~RtpH264GstNvidiaDepayloader() final;

  ///
  /// \brief Delete the copy constructor and copy assignment operator
  ///
  /// \param other
  ///
  RtpH264GstNvidiaDepayloader(const RtpH264GstNvidiaDepayloader &other) = delete;

  ///
  /// \brief Delete the copy constructor and copy assignment operator
  ///
  /// \param other
  /// \return RtpH264GstNvidiaDepayloader&
  ///
  RtpH264GstNvidiaDepayloader &operator=(RtpH264GstNvidiaDepayloader &&other) = delete;

  ///
  /// \brief Construct a new RtpH264Depayloader object (Deleted)
  ///
  /// \param other
  ///
  RtpH264GstNvidiaDepayloader(RtpH264GstNvidiaDepayloader &&other) = delete;

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
  /// \param data the fame buffer in CPU memory.
  /// \param timeout zero will wait forever or a timeout in milliseconds
  /// \return true when frame available
  /// \return false when no frame was received in the timeout
  ///
  bool Receive(::mediax::rtp::RtpFrameData *data, int32_t timeout = 0) final;

  ///
  /// \brief The callback function for the RTP stream
  ///
  /// \param frame
  ///
  void Callback(::mediax::rtp::RtpFrameData frame) const final;

  ///
  /// \brief Set new frame available
  ///
  ///
  void NewFrame();

 private:
  ///
  /// \brief The GStreamer video data callback
  ///
  /// \param appsink The GStreamer appsink
  /// \param user_data The user data
  /// \return GstFlowReturn
  ///
  static GstFlowReturn NewFrameCallback(GstAppSink *appsink, gpointer user_data);
  /// The GStreamer pipeline
  GstElement *pipeline_;
  /// A flag indication a new frame is available
  bool new_rx_frame_ = false;
};

}  // namespace mediax::rtp::h264::gst::nvidia

#endif  // H264_GST_NVIDIA_RTP_H264_DEPAYLOADER_H_
