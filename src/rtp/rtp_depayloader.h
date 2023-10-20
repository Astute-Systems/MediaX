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

#include <functional>
#include <string>
#include <vector>

#include "rtp/rtp_depayloader.h"
#include "rtp/rtp_types.h"

/// The Real Time Protocol (RTP) namespace
namespace mediax::rtp {

class RtpDepayloader;

/// \brief The RTP frame callback
using RtpCallback = std::function<void(const RtpDepayloader& depay, RtpCallbackData frame)>;

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
  /// \brief Configure at RTP input stream and dont wait for the SAP/SDP announcement
  ///
  /// \param hostname IP address i.e. 239.192.1.1 for multicast
  /// \param port defaults to 5004
  /// \param name The name of the stream
  /// \param encoding The encoding of the stream
  /// \param height The height of the stream in pixels
  /// \param width The width of the stream in pixels
  ///
  virtual void SetStreamInfo(const ::mediax::rtp::StreamInformation& stream_information) = 0;

  ///
  /// \brief Register a callback for our session_name
  ///
  /// \param session_name Advertised session name
  /// \param callback The callback to notify when a frame is received
  ///
  void RegisterCallback(const ::mediax::rtp::RtpCallback& callback);

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
  virtual void Close() = 0;

  ///
  /// \brief Recieve a frame or timeout
  ///
  /// \param cpu the fame buffer in CPU memory.
  /// \param timeout zero will wait forever or a timeout in milliseconds
  /// \return true when frame available
  /// \return false when no frame was received in the timeout
  ///
  virtual bool Receive(uint8_t** cpu, int32_t timeout = 0) = 0;

  ///
  /// \brief Set the Session Name attribute
  ///
  /// \param name The SAP/SDP session name
  ///
  void SetSessionName(std::string_view name);

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
  ::mediax::rtp::ColourspaceType GetColourSpace() const;

  ///
  /// \brief Set the Height attribute
  ///
  ///
  void SetHeight(uint32_t height);

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
  void SetWidth(uint32_t width);

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
  void SetFramerate(uint32_t framerate);

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
  void SetIpAddress(std::string_view ip_address);

  ///
  /// \brief Set the Port object
  ///
  ///
  void SetPort(uint32_t port);

  ///
  /// \brief Get the Port of the incoming stream. \note This may be invalid id no SAP/SDP announcement has been
  /// received yet.
  ///
  /// \return uint32_t
  ///
  uint32_t GetPort() const;

  ///
  /// \brief Set the Colour Space object
  ///
  /// \param colourspace
  ///
  void SetColourSpace(::mediax::rtp::ColourspaceType colourspace);

  ///
  /// \brief Get the Colour Space object
  ///
  /// \return ::mediax::rtp::ColourspaceType
  ///
  ::mediax::rtp::ColourspaceType GetColourSpace();

  ///
  /// \brief Check if the IP address is a multicast address
  ///
  /// \param ip_address
  /// \return true
  /// \return false
  ///
  bool IsMulticast(std::string_view ip_address);

  ///
  /// \brief Check is a callback is registered
  ///
  /// \return true if one is registered
  /// \return false if one is not registered
  ///
  bool CallbackRegistered() const;

  ///
  /// \brief Unregister the callback
  ///
  ///
  void UnregisterCallback();

  ///
  /// \brief Call the registered callback
  ///
  /// \param frame the callback frame data
  ///
  virtual void Callback(::mediax::rtp::RtpCallbackData frame) const = 0;

  ///
  /// \brief Get the Port Type object
  ///
  /// \return ::mediax::rtp::RtpPortType&
  ///
  ::mediax::rtp::RtpPortType& GetStream();

  /// The callback to notify when a frame is received
  ::mediax::rtp::RtpCallback callback_;

  /// UDP data buffer
  std::vector<uint8_t> buffer_in_;

 private:
  /// Indicate if callback has been registered
  bool callback_registered_ = false;
  /// Ingress port
  ::mediax::rtp::RtpPortType ingress_ = {};
};

}  // namespace mediax::rtp

#endif  // RTP_RTP_DEPAYLOADER_H_
