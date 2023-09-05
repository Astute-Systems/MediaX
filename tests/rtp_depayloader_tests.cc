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

#include "raw/rtpvraw_depayloader.h"

TEST(RTPDepayloaderTest, Copy) {
  mediax::RtpvrawDepayloader rtp;
  mediax::RtpvrawDepayloader rtp2;
  rtp = rtp2;
}

TEST(RTPDepayloaderTest, Timeout) {
  uint8_t* yuv_test;

  mediax::RtpvrawDepayloader rtp;
  rtp.SetStreamInfo("test_session_name", mediax::ColourspaceType::kColourspaceRgb24, 640, 480, "127.0.0.1", 5004);
  ASSERT_EQ(rtp.GetHeight(), 640);
  ASSERT_EQ(rtp.GetWidth(), 480);
  ASSERT_EQ(rtp.GetColourSpace(), mediax::ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtp.GetIpAddress(), "127.0.0.1");
  ASSERT_EQ(rtp.GetPort(), 5004);
  ASSERT_EQ(rtp.GetSessionName(), "test_session_name");
  rtp.Open();
  rtp.Receive(&yuv_test, 80);
  rtp.Close();
}

TEST(RTPDepayloaderTest, Many) {
  std::array<std::string, 10> ip_pool = {"239.192.1.1", "239.192.1.2", "239.192.1.3", "239.192.1.4", "239.192.1.5",
                                         "239.192.1.6", "239.192.1.7", "239.192.1.8", "239.192.1.9", "239.192.1.10"};
  std::array<mediax::RtpvrawDepayloader, 10> rtp;
  for (int i = 0; i < 10; i++) {
    LOG(INFO) << "Creating stream number " << i << " with IP:" << ip_pool[i];
    rtp[i].SetStreamInfo("test_session_name_" + std::to_string(i), mediax::ColourspaceType::kColourspaceRgb24, 640, 480,
                         ip_pool[i], 5004);
  }

  for (int i = 0; i < 10; i++) {
    LOG(INFO) << "Opening stream number " << rtp[i].GetIpAddress() << "\n";
    rtp[i].Open();
  }
}
