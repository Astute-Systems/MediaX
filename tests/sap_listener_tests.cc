//
// Copyright (c) 2024, DefenceX PTY LTD
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

mediax::sap::SdpMessage callback_data = {};

void SapTest(mediax::rtp::ColourspaceType colouspace, std::string sampling) {
  mediax::sap::SapListener& listener = mediax::sap::SapListener::GetInstance();
  std::string session_name = "test_session_name";
  static bool callback_called = false;
  mediax::sap::SapCallback callback = [&](const mediax::sap::SdpMessage* message [[maybe_unused]], void* data) {
    callback_called = true;
    callback_data = *message;
  };
  listener.RegisterSapListener(session_name, callback, nullptr);
  listener.Start();

  /// Now make an announcement
  mediax::sap::SapAnnouncer& announcer = mediax::sap::SapAnnouncer::GetInstance();
  announcer.DeleteAllSapAnnouncements();
  mediax::rtp::StreamInformation message = {"test_session_name", "127.0.0.1", 5004, 1080, 1920, 30, colouspace, false};
  announcer.AddSapAnnouncement(message);
  announcer.Start();
  ASSERT_EQ(announcer.GetActiveStreamCount(), 1);

  /// Wait for the callback to be called
  sleep(2);

  ASSERT_TRUE(callback_called);
  ASSERT_EQ(callback_data.session_name, "test_session_name");
  ASSERT_EQ(callback_data.ip_address, "127.0.0.1");
  ASSERT_EQ(callback_data.port, 5004);
  ASSERT_EQ(callback_data.framerate, 30);
  ASSERT_EQ(callback_data.sampling, sampling);
  ASSERT_EQ(callback_data.deleted, false);

  listener.Stop();
  announcer.DeleteAllSapAnnouncements();
  announcer.Stop();
}

TEST(SAPListenerTest, CheckYuv) { SapTest(mediax::rtp::ColourspaceType::kColourspaceYuv, "YCbCr-4:2:2"); }

TEST(SAPListenerTest, CheckRgb24) { SapTest(mediax::rtp::ColourspaceType::kColourspaceRgb24, "RGB"); }

TEST(SAPListenerTest, CheckMono8) { SapTest(mediax::rtp::ColourspaceType::kColourspaceMono8, "Mono"); }

TEST(SAPListenerTest, CheckMono16) { SapTest(mediax::rtp::ColourspaceType::kColourspaceMono16, "Mono"); }

TEST(SAPListenerTest, CheckH264) { SapTest(mediax::rtp::ColourspaceType::kColourspaceH264Part10, "H264"); }

TEST(SAPListenerTest, CheckH265) { SapTest(mediax::rtp::ColourspaceType::kColourspaceH265, "H265"); }

TEST(SAPListenerTest, CheckAv1) { SapTest(mediax::rtp::ColourspaceType::kColourspaceAv1, "AV1"); }

TEST(SAPListenerTest, Deleted) {
  mediax::sap::SapListener& listener = mediax::sap::SapListener::GetInstance();
  std::string session_name = "test_session_name";
  static bool callback_called = false;
  static bool deleted = false;
  mediax::sap::SapCallback callback = [&](const mediax::sap::SdpMessage* message [[maybe_unused]], void* data) {
    callback_called = true;

    if (message->deleted) {
      deleted = true;
    }
  };
  listener.RegisterSapListener(session_name, callback, nullptr);
  listener.Start();

  /// Now make an announcement
  mediax::sap::SapAnnouncer& announcer = mediax::sap::SapAnnouncer::GetInstance();
  announcer.DeleteAllSapAnnouncements();
  mediax::rtp::StreamInformation message = {
      "test_session_name", "127.0.0.1", 5004, 1920, 1080, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  announcer.AddSapAnnouncement(message);
  announcer.Start();
  ASSERT_EQ(announcer.GetActiveStreamCount(), 1);

  /// Wait for the callback to be called
  sleep(2);

  ASSERT_TRUE(callback_called);
  announcer.DeleteAllSapAnnouncements();
  // Teset deleted value for 2 seconds sample every 100ms
  for (int i = 0; i < 20; i++) {
    if (deleted) break;
    usleep(100000);
  }
  ASSERT_TRUE(deleted);
  listener.Stop();
  announcer.Stop();
}