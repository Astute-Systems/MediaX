//
// Copyright (C) 2023 DefenceX PTY LTD. All rights reserved.
//
// This software is distributed under the included copyright license.
// Any redistribution or reproduction, in part or in whole, in any form or medium, is strictly prohibited without the
// prior written consent of DefenceX PTY LTD.
//
// For any inquiries or concerns, please contact:
// DefenceX PTY LTD
// Email: enquiries@defencex.ai
//
/// \file sap_listner.cc

#ifndef __SAP_LISTNER_H__
#define __SAP_LISTNER_H__

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>

#include <functional>
#include <map>
#include <string>
#include <thread>

#include "rtp_types.h"

namespace sap {

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

// A simplified SDP message structure
// sap_text contains the complete SDP announcement
struct SDPMessage {
  uint32_t protocol_version;
  std::string session_name;
  std::string ip_address_source;
  std::string ip_address;
  uint32_t port;
  uint32_t height;
  uint32_t width;
  std::string sampling;
  uint32_t framerate;
  bool deleted = false;
  std::string sdp_text;
};

typedef std::function<void(sap::SDPMessage &sdp)> SapCallback;

class SAPListener {
 public:
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
  const std::map<std::string, SDPMessage> &GetSAPAnnouncements() const;

  ///
  /// \brief Register a callback for our session_name
  ///
  /// \param session_name Advertised session name
  ///
  void RegisterSapListener(std::string_view session_name, SapCallback callback);

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
  /// \brief Function to broadcast SAP announcements for a list of streams
  ///
  /// Only needs to be called once to start the broadcast thread
  ///
  /// \param streams
  ///
  static void SAPListenerThread(SAPListener *sap);

  ///
  /// \brief Get the SDP attribute type doe a single SDP line of text
  ///
  /// \return SdpTypeEnum attribute enum
  ///
  SdpTypeEnum GetType(std::string &line) const;

  ///
  /// \brief Parse any attributes
  ///
  /// \param line
  /// \return std::map<std::string, std::string>
  ///
  std::map<std::string, std::string> ParseAttributes(std::string &line);

  ///
  /// \brief Parse attributes with equals
  ///
  /// \param line
  /// \return std::map<std::string, std::string>
  ///
  std::map<std::string, std::string> ParseAttributesEqual(std::string &line);

  ///
  /// \brief Store or update the SAP data
  ///
  /// \return true
  /// \return false
  ///
  bool SapStore(std::array<uint8_t, kMaxUdpData> &udpdata);

  std::map<std::string, SapCallback> callbacks_;
  std::map<std::string, SDPMessage> announcements_;
  std::array<uint8_t, kMaxUdpData> udpdata;
  std::thread thread_;
  uint32_t source_ipaddress_;
  int sockfd_;
  struct sockaddr_in multicast_addr_;
  static bool running_;
};

}  // namespace sap

#endif  // __SAP_ANNOUNCER_H__
