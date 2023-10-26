//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \file sap_announcer_tests.cc
///

#include <gtest/gtest.h>
#include <unistd.h>

#include "rtp/rtp.h"
#include "sap/sap_announcer.h"

TEST(SapAnnouncerTest, AddSapAnnouncement) {
  mediax::sap::SapAnnouncer &announcer = mediax::sap::SapAnnouncer::GetInstance();
  announcer.DeleteAllSapAnnouncements();
  ASSERT_EQ(announcer.GetActiveStreamCount(), 0);
  mediax::rtp::StreamInformation message = {
      "HD Stream", "239.192.5.2", 5004, 1920, 1080, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  announcer.AddSapAnnouncement(message);
  ASSERT_EQ(announcer.GetActiveStreamCount(), 1);
}

TEST(SapAnnouncerTest, AddMultipleSAPAnnouncements) {
  mediax::sap::SapAnnouncer &announcer = mediax::sap::SapAnnouncer::GetInstance();
  announcer.DeleteAllSapAnnouncements();
  mediax::rtp::StreamInformation message1 = {
      "HD Stream", "239.192.5.2", 5004, 1920, 1080, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  mediax::rtp::StreamInformation message2 = {
      "SD Stream", "239.192.6.1", 5004, 1280, 720, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  announcer.AddSapAnnouncement(message1);
  announcer.AddSapAnnouncement(message2);
  ASSERT_EQ(announcer.GetActiveStreamCount(), 2);
}

TEST(SapAnnouncerTest, DeleteAllSAPAnnouncements) {
  mediax::sap::SapAnnouncer &announcer = mediax::sap::SapAnnouncer::GetInstance();
  announcer.DeleteAllSapAnnouncements();
  mediax::rtp::StreamInformation message1 = {
      "HD Stream", "239.192.5.2", 5004, 1920, 1080, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  mediax::rtp::StreamInformation message2 = {
      "SD Stream 1", "239.192.6.1", 5004, 1280, 720, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  mediax::rtp::StreamInformation message3 = {
      "SD Stream 2", "239.192.6.2", 5004, 1280, 720, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  mediax::rtp::StreamInformation message4 = {
      "SD Stream 3", "239.192.6.3", 5004, 1280, 720, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  mediax::rtp::StreamInformation message5 = {
      "SD Stream 4", "239.192.6.4", 5004, 1280, 720, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  mediax::rtp::StreamInformation message6 = {
      "SD Stream 5", "239.192.6.5", 5004, 1280, 720, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  mediax::rtp::StreamInformation message7 = {
      "SD Stream 6", "239.192.6.6", 5004, 1280, 720, 30, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  announcer.AddSapAnnouncement(message1);
  announcer.AddSapAnnouncement(message2);
  announcer.AddSapAnnouncement(message3);
  announcer.AddSapAnnouncement(message4);
  announcer.AddSapAnnouncement(message5);
  announcer.AddSapAnnouncement(message6);
  announcer.AddSapAnnouncement(message7);
  ASSERT_EQ(announcer.GetActiveStreamCount(), 7);
  announcer.DeleteAllSapAnnouncements();
  ASSERT_EQ(announcer.GetActiveStreamCount(), 0);
}
