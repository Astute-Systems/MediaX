//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \file rtp_depayloader_tests.cc
///

#include <gtest/gtest.h>
#include <unistd.h>

#include "uncompressed/rtp_uncompressed_payloader.h"

TEST(RTPDepayloaderTest, SendOneFrameRgb24) {
  const int kBuffSize = (640 * 480) * 3;
  std::array<uint8_t, kBuffSize> rtb_test;

  mediax::RtpUncompressedPayloader rtp;
  mediax::rtp::StreamInformation stream_information = {
      "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::rtp::ColourspaceType::kColourspaceRgb24, false};
  rtp.SetStreamInfo(stream_information);
  ASSERT_EQ(rtp.GetHeight(), 640);
  ASSERT_EQ(rtp.GetWidth(), 480);
  ASSERT_EQ(rtp.GetColourSpace(), mediax::rtp::ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtp.GetIpAddress(), "127.0.0.1");
  ASSERT_EQ(rtp.GetPort(), 5004);
  rtp.Open();
  rtp.Transmit(rtb_test.data(), true);
  rtp.Close();
}

TEST(RTPDepayloaderTest, SendOneFrameYuv) {
  const int kBuffSize = (640 * 480) * 2;
  std::array<uint8_t, kBuffSize> rtb_test;

  mediax::RtpUncompressedPayloader rtp;
  mediax::rtp::StreamInformation stream_information = {
      "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::rtp::ColourspaceType::kColourspaceYuv, false};
  rtp.SetStreamInfo(stream_information);
  ASSERT_EQ(rtp.GetHeight(), 640);
  ASSERT_EQ(rtp.GetWidth(), 480);
  ASSERT_EQ(rtp.GetColourSpace(), mediax::rtp::ColourspaceType::kColourspaceYuv);
  ASSERT_EQ(rtp.GetIpAddress(), "127.0.0.1");
  ASSERT_EQ(rtp.GetPort(), 5004);
  rtp.Open();
  rtp.Transmit(rtb_test.data(), true);
  rtp.Close();
}

TEST(RTPDepayloaderTest, SendOneFrameMono16) {
  const int kBuffSize = (640 * 480) * 2;
  std::array<uint8_t, kBuffSize> rtb_test;

  mediax::RtpUncompressedPayloader rtp;
  mediax::rtp::StreamInformation stream_information = {
      "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::rtp::ColourspaceType::kColourspaceMono16, false};
  rtp.SetStreamInfo(stream_information);
  ASSERT_EQ(rtp.GetHeight(), 640);
  ASSERT_EQ(rtp.GetWidth(), 480);
  ASSERT_EQ(rtp.GetColourSpace(), mediax::rtp::ColourspaceType::kColourspaceMono16);
  ASSERT_EQ(rtp.GetIpAddress(), "127.0.0.1");
  ASSERT_EQ(rtp.GetPort(), 5004);
  rtp.Open();
  rtp.Transmit(rtb_test.data(), true);
  rtp.Close();
}

TEST(RTPDepayloaderTest, SendOneFrameMono8) {
  const int kBuffSize = (640 * 480) * 1;
  std::array<uint8_t, kBuffSize> rtb_test;

  mediax::RtpUncompressedPayloader rtp;
  mediax::rtp::StreamInformation stream_information = {
      "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::rtp::ColourspaceType::kColourspaceMono8};
  rtp.SetStreamInfo(stream_information);
  ASSERT_EQ(rtp.GetHeight(), 640);
  ASSERT_EQ(rtp.GetWidth(), 480);
  ASSERT_EQ(rtp.GetColourSpace(), mediax::rtp::ColourspaceType::kColourspaceMono8);
  ASSERT_EQ(rtp.GetIpAddress(), "127.0.0.1");
  ASSERT_EQ(rtp.GetPort(), 5004);
  rtp.Open();
  rtp.Transmit(rtb_test.data(), true);
  rtp.Close();
}