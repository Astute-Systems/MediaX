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
/// \file rtp_depayloader.h
///

#ifndef RTP_RTP_DEPAYLOADER_H_
#define RTP_RTP_DEPAYLOADER_H_

#include <string>

#include "rtp/rtp_types.h"

namespace mediax {

///
/// \brief Manage an RTP stream
///
///
class RtpDepayloader {
 public:
  ///
  /// \brief Destroy the Rtp Stream object
  ///
  ///
  virtual ~RtpDepayloader() = default;

  ///
  /// \brief Open the RTP stream
  ///
  /// \return true
  /// \return false
  ///
  virtual bool Open() = 0;

  ///
  /// \brief Start the stream
  ///
  ///
  virtual void Start() = 0;

  ///
  /// \brief Stop the stream, can be quickly re-started
  ///
  ///
  virtual void Stop() = 0;

  ///
  /// \brief Close the RTP stream
  ///
  ///
  virtual void Close() const = 0;

  ///
  /// \brief Recieve a frame or timeout
  ///
  /// \param cpu the fame buffer in CPU memory.
  /// \param timeout zero will wait forever or a timeout in milliseconds
  /// \return true when frame available
  /// \return false when no frame was received in the timeout
  ///
  virtual bool Receive(uint8_t **cpu, int32_t timeout = 0) = 0;

  ///
  /// \brief Set the Session Name attribute
  ///
  /// \param name The SAP/SDP session name
  ///
  void SetSessionName(std::string_view name) const;

  ///
  /// \brief Get the Session Name attribute
  ///
  /// \return std::string The SAP/SDP session name
  ///
  std::string GetSessionName() const;

  ///
  /// \brief Get the Colour Space object of the incoming stream. \note This may be invalid id no SAP/SDP announcement
  /// has been received yet.
  ///
  /// \return ColourspaceType
  ///
  ColourspaceType GetColourSpace() const;

  ///
  /// \brief Set the Height attribute
  ///
  ///
  void SetHeight(uint32_t height) const;

  ///
  /// \brief Get the Height object of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return uint32_t
  ///
  uint32_t GetHeight() const;

  ///
  /// \brief Set the Width attribute
  ///
  ///
  void SetWidth(uint32_t width) const;

  ///
  /// \brief Get the Width object of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return uint32_t
  ///
  uint32_t GetWidth() const;

  ///
  /// \brief Set the Frame Rate object
  ///
  ///
  void SetFramerate(uint32_t framerate) const;

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
  /// \brief Set the Ip Address attribute
  ///
  /// \param ip_address The IPV4 address of the video stream
  ///
  void SetIpAddress(std::string_view ip_address) const;

  ///
  /// \brief Set the Port object
  ///
  ///
  void SetPort(uint32_t port) const;

  ///
  /// \brief Get the Port of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return uint32_t
  ///
  uint32_t GetPort() const;

  /// Ingress port
  static PortType ingress_;
};

}  // namespace mediax

#endif  // RTP_RTP_DEPAYLOADER_H_
