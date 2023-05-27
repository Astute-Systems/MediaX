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

#include "sap_announcer.h"

TEST(SAPAnnouncerTest, AddSAPAnnouncement) {
  sap::SAPAnnouncer &announcer = sap::SAPAnnouncer::GetInstance();
  sap::SAPMessage message = {"HD Stream", "239.192.5.2", 5004, 1920, 1080, 30, false};
  announcer.AddSAPAnnouncement(message);
  ASSERT_EQ(announcer.GetActiveStreamCount(), 1);
}

TEST(SAPAnnouncerTest, AddMultipleSAPAnnouncements) {
  sap::SAPAnnouncer &announcer = sap::SAPAnnouncer::GetInstance();
  announcer.DeleteAllSAPAnnouncements();
  sap::SAPMessage message1 = {"HD Stream", "239.192.5.2", 5004, 1920, 1080, 30, false};
  sap::SAPMessage message2 = {"SD Stream", "239.192.6.1", 5004, 1280, 720, 30, false};
  announcer.AddSAPAnnouncement(message1);
  announcer.AddSAPAnnouncement(message2);
  ASSERT_EQ(announcer.GetActiveStreamCount(), 2);
}

TEST(SAPAnnouncerTest, DeleteAllSAPAnnouncements) {
  sap::SAPAnnouncer &announcer = sap::SAPAnnouncer::GetInstance();
  announcer.DeleteAllSAPAnnouncements();
  sap::SAPMessage message1 = {"HD Stream", "239.192.5.2", 5004, 1920, 1080, 30, false};
  sap::SAPMessage message2 = {"SD Stream 1", "239.192.6.1", 5004, 1280, 720, 30, false};
  sap::SAPMessage message3 = {"SD Stream 2", "239.192.6.2", 5004, 1280, 720, 30, false};
  sap::SAPMessage message4 = {"SD Stream 3", "239.192.6.3", 5004, 1280, 720, 30, false};
  sap::SAPMessage message5 = {"SD Stream 4", "239.192.6.4", 5004, 1280, 720, 30, false};
  sap::SAPMessage message6 = {"SD Stream 5", "239.192.6.5", 5004, 1280, 720, 30, false};
  sap::SAPMessage message7 = {"SD Stream 6", "239.192.6.6", 5004, 1280, 720, 30, false};
  announcer.AddSAPAnnouncement(message1);
  announcer.AddSAPAnnouncement(message2);
  announcer.AddSAPAnnouncement(message3);
  announcer.AddSAPAnnouncement(message4);
  announcer.AddSAPAnnouncement(message5);
  announcer.AddSAPAnnouncement(message6);
  announcer.AddSAPAnnouncement(message7);
  ASSERT_EQ(announcer.GetActiveStreamCount(), 7);
  announcer.DeleteAllSAPAnnouncements();
  ASSERT_EQ(announcer.GetActiveStreamCount(), 0);
}
