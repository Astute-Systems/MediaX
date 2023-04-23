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

#include <stdint.h>

#include <string>
#include <vector>

namespace sap {

struct SAPHeader {
  uint8_t version;              // Protocol version
  uint8_t hash;                 // Hash, must be unique for this announcement
  uint32_t originating_source;  // Originating Source, IPV4

  // Constructor
  SAPHeader(uint8_t version, uint8_t hash, uint16_t originating_source)
      : version(version), hash(hash), originating_source(originating_source) {}
};

// A simplified SAP message structure
struct SAPMessage {
  std::string sessionName;
  std::string ipAddress;
  uint32_t port;
  uint32_t height;
  uint32_t width;
  uint32_t framerate;
};

class SAPAnnouncer {
 public:
  ///
  /// \brief Add a stream announcement
  ///
  /// \param message The stream details
  ///
  void AddSAPAnnouncement(const SAPMessage &message);

  ///
  /// \brief  Function to send a SAP announcement
  ///
  /// \param message The stream details
  ///
  void SendSAPAnnouncement(const SAPMessage &message);

  ///
  /// \brief Function to broadcast SAP announcements for a list of streams
  ///
  /// \param streams
  ///
  void BroadcastSAPAnnouncements();

  void ListAddresses(uint16_t select = 0);

 private:
  std::vector<SAPMessage> streams_;
  const std::string kIpaddr = "224.2.127.254";
  const uint32_t kPort = 9875;
  uint32_t source_ipaddress_;
};

}  // namespace sap

#endif  // __SAP_ANNOUNCER_H__
