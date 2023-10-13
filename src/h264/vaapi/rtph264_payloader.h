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
/// \file rtph264_payloader.h
///

#ifndef H264_VAAPI_RTPH264_PAYLOADER_H_
#define H264_VAAPI_RTPH264_PAYLOADER_H_

#include <gst/gst.h>

#include "rtp/rtp_payloader.h"

namespace mediax::rtp::h264::vaapi {

class RtpH264VaapiPayloader : public RtpPayloader {
  ///
  /// \brief Construct a new Rtp H.264 Payloader object
  ///
  ///
  RtpH264VaapiPayloader();

  ///
  /// \brief Destroy the Rtp H.264 Payloader object
  ///
  ///
  ~RtpH264VaapiPayloader() final;

  ///
  /// \brief Set the Stream Info object
  ///
  /// \param name session name
  /// \param encoding colour space
  /// \param height height in pixels
  /// \param width width in pixels
  /// \param hostname IP address i.e. 123.192.1.1
  /// \param portno port number i.e. 5004
  ///
  void SetStreamInfo(std::string_view name, ColourspaceType encoding, uint32_t height, uint32_t width,
                     uint32_t framerate, std::string_view hostname, const uint32_t portno) override;

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
  /// \brief
  ///
  /// \param rgbframe pointer to RGB buffer
  /// \param blocking set to true if blocking
  /// \return int error if value is <0, 0 on success
  ///
  int Transmit(uint8_t *rgbframe, bool blocking = true) final;

  ///
  /// \brief Stop the stream, can be quickly re-started
  ///
  ///
  void Start() final;

  ///
  /// \brief Stop the stream
  ///
  ///
  void Stop() final;

 private:
  GstElement *pipeline_;
};

}  // namespace mediax::rtp::h264::vaapi

#endif  // H264_VAAPI_RTPH264_PAYLOADER_H_
