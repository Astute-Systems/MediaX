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

#include <string>
#include <thread>
#include <vector>

namespace sap {

// A simplified SDP message structure
// sap_text contains the complete SDP announcement
struct SDPMessage {
  std::string sessionName;
  std::string ipAddress;
  uint16_t port;
  uint32_t height;
  uint32_t width;
  uint32_t framerate;
  bool deleted = false;
  std::string sdp_text;
};

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
  const std::vector<SDPMessage> &GetSAPAnnouncements();

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

  std::vector<SDPMessage> announcements_;
};

}  // namespace sap

#endif  // __SAP_ANNOUNCER_H__
