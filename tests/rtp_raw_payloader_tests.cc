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

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include "rtp/rtp_utils.h"
#include "uncompressed/rtp_uncompressed_payloader.h"

TEST(RTPDepayloaderTest, SendOneFrameRgb24) {
  const int kBuffSize = (640 * 480) * 3;
  std::array<uint8_t, kBuffSize> rtb_test;

  mediax::rtp::uncompressed::RtpUncompressedPayloader rtp;
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

  mediax::rtp::uncompressed::RtpUncompressedPayloader rtp;
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

  mediax::rtp::uncompressed::RtpUncompressedPayloader rtp;
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

  mediax::rtp::uncompressed::RtpUncompressedPayloader rtp;
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

TEST(RTPDepayloaderTest, Many) {
  std::array<std::string, 10> ip_pool = {"239.192.1.1", "239.192.1.2", "239.192.1.3", "239.192.1.4", "239.192.1.5",
                                         "239.192.1.6", "239.192.1.7", "239.192.1.8", "239.192.1.9", "239.192.1.10"};
  std::array<mediax::rtp::uncompressed::RtpUncompressedPayloader, 10> rtp;
  for (int i = 0; i < 10; i++) {
    LOG(INFO) << "Creating stream number " << i << " with IP:" << ip_pool[i];
    mediax::rtp::StreamInformation stream_info = {
        "test_session_name_" + std::to_string(i),         ip_pool[i], 5004, 640, 480, 25,
        mediax ::rtp::ColourspaceType::kColourspaceRgb24, false};
    rtp[i].SetStreamInfo(stream_info);
  }

  // Open and Start
  for (int i = 0; i < 10; i++) {
    LOG(INFO) << "Opening stream number " << rtp[i].GetIpAddress() << "\n";
    rtp[i].Open();
    rtp[i].Start();
  }

  // Transmit one frame
  std::vector<uint8_t> buffer(640 * 480 * 3);
  CreateCheckeredTestCard(buffer.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  for (int i = 0; i < 10; i++) {
    rtp[i].Transmit(buffer.data(), 80);
  }

  // Stop and Close
  for (int i = 0; i < 10; i++) {
    LOG(INFO) << "Opening stream number " << rtp[i].GetIpAddress() << "\n";
    rtp[i].Stop();
    rtp[i].Close();
  }
}
