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
/// \file rtpv_payloader.h
///

#ifndef __RTP_PAYLOADER_H
#define __RTP_PAYLOADER_H

#include <string>

#include "rtp_types.h"

///
/// \brief Manage an RTP stream
///
///
class RtpPayloader {
 public:
  ///
  /// \brief Destroy the Rtp Payloader object
  ///
  ///
  virtual ~RtpPayloader() = default;

  ///
  /// \brief Configure an RTP output stream
  ///
  /// \param hostname IP address i.e. 239.192.1.1 for multicast
  /// \param port defaults to 5004
  /// \param name The name of the stream
  /// \param encoding The colour space of the stream
  /// \param height The height of the stream in pixels
  /// \param width The width of the stream in pixels
  ///
  virtual void SetStreamInfo(std::string_view name, ColourspaceType encoding, uint32_t height, uint32_t width,
                             std::string_view hostname, const uint32_t port = 5004) = 0;

  ///
  /// \brief Open the RTP stream
  ///
  /// \return true
  /// \return false
  ///
  virtual bool Open() = 0;

  ///
  /// \brief Close the RTP stream
  ///
  ///
  virtual void Close() = 0;

  ///
  /// \brief Stop a stream so it can be quickly restarted. Do not close
  ///
  ///
  virtual void Stop();

  ///
  /// \brief Restart a stopped stream
  ///
  ///
  virtual void Start();

  ///
  /// \brief Transmit an RGB buffer
  ///
  /// \param rgbframe pointer to the frame data
  /// \param blocking defaults to true, will wait till frame has been transmitted
  /// \return int
  ///
  virtual int Transmit(uint8_t *rgbframe, bool blocking = true) = 0;

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

  /// Egress port
  PortType egress_;
};

#endif  // __RTP_PAYLOADER_H
