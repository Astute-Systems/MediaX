//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief Session Announcement Protocol (SDP) implementation for listening to announcements of stream data. The SAP
/// packet contains the Session Description Protocol (SDP).
///
/// \file sap_listener.h
///

#ifndef SAP_SAP_LISTENER_H_
#define SAP_SAP_LISTENER_H_

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include <stdint.h>
#include <unistd.h>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include "rtp/rtp_types.h"

namespace mediax::sap {

/// The SDP type as per RFC8866:
///     v=  (protocol version)
///     o=  (originator and session identifier)
///     s=  (session name)
///     i=* (session information)
///     u=* (URI of description)
///     e=* (email address)
///     p=* (phone number)
///     c=* (connection information -- not required if included in
///          all media descriptions)
///     b=* (zero or more bandwidth information lines)
///     One or more time descriptions:
///       ("t=", "r=" and "z=" lines; see below)
///     k=* (obsolete)
///     a=* (zero or more session attribute lines)
///     Zero or more media descriptions

//  Time description
//     t=  (time the session is active)
//     r=* (zero or more repeat times)
//     z=* (optional time zone offset line)

//  Media description, if present
//     m=  (media name and transport address)
//     i=* (media title)
//     c=* (connection information -- optional if included at
//          session level)
//     b=* (zero or more bandwidth information lines)
//     k=* (obsolete)
//     a=* (zero or more media attribute lines)
enum class SdpTypeEnum {
  kProtocolVersion,
  kOriginatorSessionIdentifier,
  kSessionName,
  kSessionInformation,
  kUriOfDescription,
  kEmailAddress,
  kPhoneNumber,
  kConnectionInformation,
  kBandwidthInformation,
  kSessionAttribute,
  kTimeSessionActive,
  kMediaNameAndTransportAddress,
  kMediaTitle,
  KConnectionInformation,
  kUnknown
};

/// A simplified SDP message structure
/// \brief sap_text contains the complete SDP announcement
struct SDPMessage {
  /// Protocol version
  uint32_t protocol_version;
  /// Originator and session identifier
  std::string session_name;
  /// Session IPV4 source address
  std::string ip_address_source;
  /// Session IPV4 destination address
  std::string ip_address;
  /// The IPV4 source port
  uint32_t port;
  /// The height of the video stream
  uint32_t height;
  /// The width of the video stream
  uint32_t width;
  /// The sampling rate of the video stream
  std::string sampling;
  /// The framerate of the video stream
  uint32_t framerate;
  /// This bits per pixel of the video stream
  uint32_t bits_per_pixel;
  /// Set to true if the SDP message has been deleted
  bool deleted = false;
  /// The complete SDP announcement
  std::string sdp_text;
};

/// \brief The SAPListener class is a singleton that listens for SAP/SDP announcements on the network
using SapCallback = std::function<void(sap::SDPMessage *sdp)>;

/// Class definition of the SAPListener
class SAPListener {
 public:
  ///
  /// \brief Construct a new SAPListener::SAPListener object
  ///
  ///
  SAPListener();

  ///
  /// \brief Destroy the SAPListener::SAPListener object
  ///
  ///
  ~SAPListener();

  ///
  /// \brief A Singleton get method
  ///
  ///
  static SAPListener &GetInstance();

  ///
  /// \brief Get a list of SAP/SDP streams seen on the network
  ///
  /// \return The vectored list of SAP/SDP streams seen on the network
  ///
  const std::map<std::string, ::mediax::sap::SDPMessage, std::less<>> &GetSAPAnnouncements() const;

  ///
  /// \brief Register a callback for our session_name
  ///
  /// \param session_name Advertised session name
  /// \param callback The callback to notify when SAP/SDP message received
  ///
  void RegisterSapListener(std::string_view session_name, const ::mediax::sap::SapCallback &callback);

  ///
  /// \brief Get the stream information for a given session name
  ///
  /// \param session_name
  /// \return StreamInformation
  ///
  ::mediax::StreamInformation GetStreamInformation(std::string_view session_name) const;

  ///
  /// \brief Start the SAP/SDP announcements thread
  ///
  ///
  void Start();

  ///
  /// \brief Stop the SAP/SDP announcement thread
  ///
  ///
  void Stop();

 private:
  /// The pointer to the SAPListener singleton
  static std::unique_ptr<SAPListener> singleton_;

  ///
  /// \brief Function to broadcast SAP announcements for a list of streams
  ///
  /// Only needs to be called once to start the broadcast thread
  ///
  /// \param sap SAPListener object
  ///
  static void SAPListenerThread(SAPListener *sap);

  ///
  /// \brief Get the SDP attribute type doe a single SDP line of text
  ///
  /// \return SdpTypeEnum attribute enum
  ///
  SdpTypeEnum GetType(const std::string_view &line) const;

  ///
  /// \brief Parse any attributes
  ///
  /// \param line
  /// \return std::map<std::string, std::string>
  ///
  std::map<std::string, std::string, std::less<>> ParseAttributes(const std::string_view &line) const;

  ///
  /// \brief Parse attributes with equals
  ///
  /// \param line
  /// \return std::map<std::string, std::string>
  ///
  std::map<std::string, std::string, std::less<>> ParseAttributesEqual(const std::string &line) const;

  ///
  /// \brief Store or update the SAP data
  ///
  /// \return true
  /// \return false
  ///
  bool SapStore(std::array<uint8_t, kMaxUdpData> *udpdata);

  /// The list of SAP/SDP announcement callbacks
  std::map<std::string, SapCallback, std::less<>> callbacks_;
  /// The list of SAP/SDP announcements
  std::map<std::string, SDPMessage, std::less<>> announcements_;
  /// The buffer for the UDP data
  std::array<uint8_t, kMaxUdpData> udpdata_;
  /// The thread for the SAP/SDP announcements
  std::thread thread_;
  /// The socket for the SAP/SDP announcements
  int sockfd_;
  /// The multicast address for the SAP/SDP announcements
  struct sockaddr_in multicast_addr_;
  /// The SAP/SDP announcements thread running flag
  static bool running_;
};

}  // namespace mediax::sap

#endif  // SAP_SAP_LISTENER_H_
