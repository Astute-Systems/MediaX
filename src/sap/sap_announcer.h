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

#ifndef SAP_SAP_ANNOUNCER_H_
#define SAP_SAP_ANNOUNCER_H_

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

#include "rtp/rtp_types.h"

// The Session Announcement Protocol (SAP) and Session Description Protocol (SDP) namespace
namespace mediax::sap {

/// Class to announce the stream details using the SAP protocol
struct SapHeader {
 public:
  /// Protocol version
  uint8_t version;
  /// Hash, must be unique for this announcement
  uint8_t hash;
  /// Originating Source, IPV4
  uint32_t originating_source;
  ///
  /// \brief Construct a new SapHeader object
  ///
  /// \param version Protocol version
  /// \param hash This must be unique for this announcement
  /// \param originating_source The originating source identity
  ///
  SapHeader(uint8_t version, uint8_t hash, uint32_t originating_source)
      : version(version), hash(hash), originating_source(originating_source) {}
};

/// Class to announce the stream details using the SAP protocol
class SapAnnouncer {
 public:
  ///
  /// \brief A Singleton get method
  ///
  ///
  static SapAnnouncer &GetInstance();

  ///
  /// \brief Construct a new SapAnnouncer object, not used so deleted
  ///
  ///
  SapAnnouncer(SapAnnouncer const &) = delete;

  ///
  /// \brief Add a stream announcement
  ///
  /// \param stream_information The stream details
  ///
  void AddSapAnnouncement(const ::mediax::rtp::StreamInformation &stream_information);

  ///
  /// \brief Delete a specific session announcement
  ///
  /// \param session_name
  ///
  void DeleteSapAnnouncement(std::string_view session_name);

  ///
  /// \brief Deletes all announcements, thread is still running. Call Stop() method to terminate the thread
  ///
  ///
  void DeleteAllSapAnnouncements();

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
  std::vector<::mediax::rtp::StreamInformation> &GetStreams();

  ///
  /// \brief Get the number of active streams
  ///
  /// \return uint32_t
  ///
  uint32_t GetActiveStreamCount() const;

 private:
  ///
  /// \brief Construct a new SapAnnouncer::SapAnnouncer object
  ///
  ///
  SapAnnouncer();

  ///
  /// \brief Destroy the SapAnnouncer::SapAnnouncer object
  ///
  ///
  ~SapAnnouncer();

  ///
  /// \brief Function to broadcast SAP announcements for a list of streams
  ///
  /// Only needs to be called once to start the broadcast thread
  ///
  /// \param stream_information The SapAnnouncer object
  ///
  static void SapAnnouncementThread(SapAnnouncer *stream_information);

  ///
  /// \brief  Function to send a SAP announcement
  ///
  /// \param stream_information The stream details
  ///
  void SendSapAnnouncement(const ::mediax::rtp::StreamInformation &stream_information) const;

  ///
  /// \brief Function to send a SAP deletion
  ///
  /// \param stream_information The stream details, needed to form the deletion packet
  ///
  void SendSapDeletion(const ::mediax::rtp::StreamInformation &stream_information) const;

  ///
  /// \brief Sed SAP packet
  ///
  /// \param stream_information
  /// \param deletion
  ///
  void SendSapPacket(const ::mediax::rtp::StreamInformation &stream_information, bool deletion) const;

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
  /// \brief Check the interface addresses
  ///
  /// \param ifa the IPV4 interface address
  /// \param helper output helper messages
  /// \param selected the selected interface
  ///
  void CheckAddresses(struct ifaddrs *ifa, bool helper, uint16_t selected);

  /// A list of active SAP streams
  std::vector<::mediax::rtp::StreamInformation> streams_;
  /// The SAP/SDP transmission thread
  std::thread thread_;
  /// The IPV4 source address
  uint32_t source_ipaddress_;
  /// The open socket file descriptor
  int sockfd_;
  /// The multicast address
  struct sockaddr_in multicast_addr_;
  /// The flag indicating the SAP thread is active
  static bool running_;
};

}  // namespace mediax::sap

#endif  // SAP_SAP_ANNOUNCER_H_
