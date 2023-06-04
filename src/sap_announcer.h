//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief Session Announcement Protocol (SDP) implementation for announcement of the stream data. The SAP packet
/// contains the Session Description Protocol (SDP).
///
/// \file sap_announcer.h
///

#ifndef __SAP_ANNOUNCER_H__
#define __SAP_ANNOUNCER_H__

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

#include <string>
#include <thread>
#include <vector>

#include "rtp_types.h"

namespace sap {

/// The SAP header structure
struct SAPHeader {
  /// Protocol version
  uint8_t version;
  /// Hash, must be unique for this announcement
  uint8_t hash;
  /// Originating Source, IPV4
  uint32_t originating_source;
  ///
  /// \brief Construct a new SAPHeader object
  ///
  /// \param version Protocol version
  /// \param hash This must be unique for this announcement
  /// \param originating_source The originating source identity
  ///
  SAPHeader(uint8_t version, uint8_t hash, uint32_t originating_source)
      : version(version), hash(hash), originating_source(originating_source) {}
};

/// A simplified SAP message structure
struct SAPMessage {
  /// The SDP session name
  std::string sessionName;
  /// The IPV4 address as a string
  std::string ipAddress;
  /// The IPV4 port number
  uint32_t port;
  /// The stream height in pixels
  uint32_t height;
  /// The stream width in pixels
  uint32_t width;
  /// The stream framerate in frames / second
  uint32_t framerate;
  /// Colourspace encoding
  ColourspaceType encoding;
  /// Flag indicating the stream was deleted
  bool deleted = false;
};

/// Class definition for the SAPAnnouncer
class SAPAnnouncer {
 public:
  ///
  /// \brief A Singleton get method
  ///
  ///
  static SAPAnnouncer &GetInstance();

  ///
  /// \brief Construct a new SAPAnnouncer object, not used so deleted
  ///
  ///
  SAPAnnouncer(SAPAnnouncer const &) = delete;

  ///
  /// \brief Add a stream announcement
  ///
  /// \param message The stream details
  ///
  void AddSAPAnnouncement(const SAPMessage &message);

  ///
  /// \brief Deletes all announcements, thread is still running. Call Stop() method to terminate the thread
  ///
  ///
  void DeleteAllSAPAnnouncements();

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

  ///
  /// \brief Set the Source Interface object
  ///
  /// \param select The interface to select as the source
  ///
  void SetSourceInterface(uint16_t select = 0);

  ///
  /// \brief Set the Source Interface object and list all interfaces to stdout
  ///
  /// \param select The interface to select as the source
  ///
  void ListInterfaces(uint16_t select = 0);

  ///
  /// \brief Get the Streams object
  ///
  /// \return std::vector<SAPMessage>&
  ///
  std::vector<SAPMessage> &GetStreams() { return streams_; }

  ///
  /// \brief Get the number of active streams
  ///
  /// \return uint32_t
  ///
  uint32_t GetActiveStreamCount() const { return (uint32_t)streams_.size(); }

 private:
  ///
  /// \brief Construct a new SAPAnnouncer::SAPAnnouncer object
  ///
  ///
  SAPAnnouncer();

  ///
  /// \brief Destroy the SAPAnnouncer::SAPAnnouncer object
  ///
  ///
  ~SAPAnnouncer();

  ///
  /// \brief Function to broadcast SAP announcements for a list of streams
  ///
  /// Only needs to be called once to start the broadcast thread
  ///
  /// \param streams
  ///
  static void SAPAnnouncementThread(SAPAnnouncer *sap);

  ///
  /// \brief  Function to send a SAP announcement
  ///
  /// \param message The stream details
  ///
  void SendSAPAnnouncement(const SAPMessage &message) const;

  ///
  /// \brief Function to send a SAP deletion
  ///
  /// \param message The stream details, needed to form the deletion packet
  ///
  void SendSAPDeletion(const SAPMessage &message) const;

  ///
  /// \brief Sed SAP packet
  ///
  /// \param message
  /// \param deletion
  ///
  void SendSAPPacket(const SAPMessage &message, bool deletion) const;

  ///
  /// \brief Set the Address Helper object, can print interface details if helper is set
  ///
  /// \param select The interface to select
  /// \param helper Print out the list of interfaces if set to true
  ///
  void SetAddressHelper(uint16_t select, bool helper);

  ///
  /// \brief Delete all the SAP/SDP streams
  ///
  ///
  void DeleteAllStreams();

  ///
  /// \brief Check all the network interfaces
  ///
  /// \param addr_str
  ///
  void CheckAddresses(struct ifaddrs *ifa, bool helper, uint16_t selected);

  std::vector<SAPMessage> streams_;
  std::thread thread_;
  uint32_t source_ipaddress_;
  int sockfd_;
  struct sockaddr_in multicast_addr_;
  static bool running_;
};

}  // namespace sap

#endif  // __SAP_ANNOUNCER_H__
