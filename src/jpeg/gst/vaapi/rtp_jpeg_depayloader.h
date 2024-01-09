
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
/// \file rtp_h265_depayloader.h
///

#ifndef JPEG_GST_VAAPI_RTP_JPEG_DEPAYLOADER_H_
#define JPEG_GST_VAAPI_RTP_JPEG_DEPAYLOADER_H_

#include <gst/app/gstappsink.h>
#include <gst/gst.h>

#include <vector>

#include "rtp/rtp_depayloader.h"

/// The JPEG video compression GStreamer namespace
namespace mediax::rtp::jpeg::gst::vaapi {

/// A RTP payloader for H.264 DEF-STAN 00-82 video streams
class RtpJpegGstVaapiDepayloader : public mediax::rtp::RtpDepayloader {
 public:
  ///
  /// \brief Construct a new Rtpvraw Depayloader object
  ///
  ///
  RtpJpegGstVaapiDepayloader();

  ///
  /// \brief Destroy the Rtpvraw Depayloader object
  ///
  ///
  ~RtpJpegGstVaapiDepayloader() final;

  ///
  /// \brief Copy operator (Deleted)
  ///
  /// \param other
  /// \return Rtph265Depayloader
  ///
  RtpJpegGstVaapiDepayloader &operator=(const RtpJpegGstVaapiDepayloader &other);

  ///
  /// \brief Construct a new Rtph265Depayloader object (Deleted)
  ///
  /// \param other
  ///
  RtpJpegGstVaapiDepayloader(RtpJpegGstVaapiDepayloader &&other) = delete;

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
  /// \param data the fame buffer in CPU memory. I
  /// \param timeout zero will wait forever or a timeout in milliseconds
  /// \return true when frame available
  /// \return false when no frame was received in the timeout and the cpu pointer is invalid
  ///
  bool Receive(mediax::rtp::RtpFrameData *data, int32_t timeout = 0) final;

  ///
  /// \brief Call the callback
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
  /// \brief GStreamer callback for new frames
  ///
  /// \param appsink The GStreamer appsink
  /// \param user_data The user data
  /// \return GstFlowReturn
  ///
  static GstFlowReturn NewFrameCallback(GstAppSink *appsink, gpointer user_data);

  /// The GStreamer pipeline
  GstElement *pipeline_;
  /// The video buffer
  std::vector<uint8_t> buffer_in_;
  /// A flag indication a new frame is available
  bool new_rx_frame_ = false;
};

}  // namespace mediax::rtp::jpeg::gst::vaapi

#endif  // JPEG_GST_VAAPI_RTP_JPEG_DEPAYLOADER_H_
