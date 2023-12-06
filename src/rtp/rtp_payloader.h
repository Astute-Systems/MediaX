//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief RTP streaming video class for uncompressed DEF-STAN 00-82 video streams
///
/// \file rtp_payloader.h
///

#ifndef RTP_RTP_PAYLOADER_H_
#define RTP_RTP_PAYLOADER_H_

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "rtp/rtp_types.h"

namespace mediax::rtp {

///
/// \brief Manage an RTP stream
///
///
class RtpPayloader {
 public:
  ///
  /// \brief Construct a new Rtp Payloader object
  ///
  ///
  RtpPayloader() = default;

  ///
  /// \brief Destroy the Rtp Payloader object
  ///
  ///
  virtual ~RtpPayloader() = default;

  ///
  /// \brief Construct a new Rtp Payloader object
  ///
  ///
  RtpPayloader(RtpPayloader const &) = delete;

  ///
  /// \brief Construct a new Rtp Payloader object
  ///
  /// \return RtpPayloader&
  ///
  RtpPayloader &operator=(RtpPayloader const &) = delete;

  ///
  /// \brief Configure an RTP output stream
  ///
  /// \param stream_information set the stream information
  ///
  virtual void SetStreamInfo(const ::mediax::rtp::StreamInformation &stream_information) = 0;

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
  mediax::rtp::ColourspaceType GetColourSpace() const;

  ///
  /// \brief Set the Colour Space object
  ///
  /// \param colourspace
  ///
  void SetColourSpace(mediax::rtp::ColourspaceType colourspace);

  ///
  /// \brief Get the Height object of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return uint32_t
  ///
  uint32_t GetHeight() const;

  ///
  /// \brief Set the Height object
  ///
  /// \param height
  ///
  void SetHeight(uint32_t height);

  ///
  /// \brief Get the Width object of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return uint32_t
  ///
  uint32_t GetWidth() const;

  ///
  /// \brief Set the Width object
  ///
  /// \param width
  ///
  void SetWidth(uint32_t width);

  ///
  /// \brief Get the Frame Rate of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return uint32_t
  ///
  uint32_t GetFrameRate() const;

  ///
  /// \brief Set the Frame Rate object
  ///
  /// \param framerate
  ///
  void SetFrameRate(uint32_t framerate);

  ///
  /// \brief Get the Ip Address of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return std::string
  ///
  std::string GetIpAddress() const;

  ///
  /// \brief Set the Ip Address object
  ///
  /// \param ip_address
  ///
  void SetIpAddress(std::string_view ip_address);

  ///
  /// \brief Get the Port of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return uint32_t
  ///
  uint32_t GetPort() const;

  ///
  /// \brief Set the Port object
  ///
  /// \param port
  ///
  void SetPort(uint32_t port);

  ///
  /// \brief Get the Buffer object
  ///
  /// \return std::vector<uint8_t>&
  ///
  std::vector<uint8_t> &GetBuffer() { return frame_; }

  ///
  /// \brief Get the Buffer Size object
  ///
  /// \return uint32_t
  ///
  uint32_t GetBufferSize() const { return static_cast<uint32_t>(frame_.size()); }

  ///
  /// \brief Set the Buffer Size object
  ///
  /// \param size
  ///
  void SetBufferSize(uint32_t size) { frame_.resize(size); }

 protected:
  ///
  /// \brief Get the Egress Port object
  ///
  /// \return ::mediax::rtp::RtpPortType&
  ///
  ::mediax::rtp::RtpPortType &GetEgressPort();

 private:
  /// Egress port
  ::mediax::rtp::RtpPortType egress_;
  /// Data buffer
  std::vector<uint8_t> frame_;
};

}  // namespace mediax::rtp

#endif  // RTP_RTP_PAYLOADER_H_
