//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \file sap_listener_tests.cc
///

#include <gtest/gtest.h>
#include <unistd.h>

#include "sap/sap_announcer.h"
#include "sap/sap_listener.h"

TEST(SAPListenerTest, RegisterSapListenerTest) {
  mediax::sap::SapListener& listener = mediax::sap::SapListener::GetInstance();
  std::string session_name = "test_session_name";
  static bool callback_called = false;
  mediax::sap::SapCallback callback = [&](const mediax::sap::SdpMessage* message [[maybe_unused]]) {
    callback_called = true;
  };
  listener.RegisterSapListener(session_name, callback);
  listener.Start();

  /// Now make an announcement
  mediax::sap::SapAnnouncer& announcer = mediax::sap::SapAnnouncer::GetInstance();
  announcer.DeleteAllSAPAnnouncements();
  mediax::rtp::StreamInformation message = {
      "test_session_name", "127.0.0.1", 5004, 1920, 1080, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  announcer.AddSapAnnouncement(message);
  announcer.Start();
  ASSERT_EQ(announcer.GetActiveStreamCount(), 1);

  /// Wait for the callback to be called
  sleep(2);

  ASSERT_TRUE(callback_called);
  listener.Stop();
  announcer.Stop();
}