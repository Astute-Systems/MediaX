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
/// \file sap_announcer.cc

#ifndef __SAP_ANNOUNCER_H__
#define __SAP_ANNOUNCER_H__

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>
#include <thread>
#include <vector>

namespace sap {

struct SAPHeader {
  uint8_t version;              // Protocol version
  uint8_t hash;                 // Hash, must be unique for this announcement
  uint32_t originating_source;  // Originating Source, IPV4

  // Constructor
  SAPHeader(uint8_t version, uint8_t hash, uint32_t originating_source)
      : version(version), hash(hash), originating_source(originating_source) {}
};

// A simplified SAP message structure
struct SAPMessage {
  std::string sessionName;
  std::string ipAddress;
  uint16_t port;
  uint32_t height;
  uint32_t width;
  uint32_t framerate;
  bool deleted = false;
};

class SAPAnnouncer {
 public:
  ///
  /// \brief A Singleton get method
  ///
  ///
  static SAPAnnouncer &GetInstance();

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

  std::vector<SAPMessage> &GetStreams() { return streams_; }

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
  const std::string kIpaddr = "224.2.127.254";
  const uint16_t kPort = 9875;
  uint32_t source_ipaddress_;
  int sockfd_;
  struct sockaddr_in multicast_addr_;
  static bool running_;
};

}  // namespace sap

#endif  // __SAP_ANNOUNCER_H__
