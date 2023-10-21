
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
/// \file rtph265_depayloader.h
///

#ifndef H265_GST_VAAPI_RTP_H265_DEPAYLOADER_H_
#define H265_GST_VAAPI_RTP_H265_DEPAYLOADER_H_

#include <gst/app/gstappsink.h>
#include <gst/gst.h>

#include <vector>

#include "rtp/rtp_depayloader.h"

/// The H.264 video compression GStreamer namespace
namespace mediax::rtp::h265::gst {}
/// The Gstreamer implementation of the Nvidia (NVENC) namespace
namespace mediax::rtp::h265::gst::vaapi {

GstFlowReturn NewFrameCallback(GstAppSink *appsink, gpointer user_data);

class RtpH265GstVaapiDepayloader : public mediax::rtp::RtpDepayloader {
 public:
  ///
  /// \brief Construct a new Rtpvraw Depayloader object
  ///
  ///
  RtpH265GstVaapiDepayloader();

  ///
  /// \brief Destroy the Rtpvraw Depayloader object
  ///
  ///
  ~RtpH265GstVaapiDepayloader() final;

  ///
  /// \brief Copy operator (Deleted)
  ///
  /// \param other
  /// \return Rtph265Depayloader
  ///
  RtpH265GstVaapiDepayloader &operator=(const RtpH265GstVaapiDepayloader &other);

  ///
  /// \brief Construct a new Rtph265Depayloader object (Deleted)
  ///
  /// \param other
  ///
  RtpH265GstVaapiDepayloader(RtpH265GstVaapiDepayloader &&other) = delete;

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
  /// \param cpu the fame buffer in CPU memory. I
  /// \param timeout zero will wait forever or a timeout in milliseconds
  /// \return true when frame available
  /// \return false when no frame was received in the timeout and the cpu pointer is invalid
  ///
  bool Receive(uint8_t **cpu, int32_t timeout = 0) final;

  ///
  /// \brief Call the callback
  ///
  /// \param frame
  ///
  void Callback(::mediax::rtp::RtpCallbackData frame) const final;

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

}  // namespace mediax::rtp::h265::gst::vaapi

#endif  // H265_GST_VAAPI_RTP_H265_DEPAYLOADER_H_
