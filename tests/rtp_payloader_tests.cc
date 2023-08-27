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

#include "raw/rtpvraw_payloader.h"

TEST(RTPDepayloaderTest, SendOneFrame) {
  const int kBuffSize = (640 * 480) * 3;
  std::array<uint8_t, kBuffSize> rtb_test;

  RtpvrawPayloader rtp;
  rtp.SetStreamInfo("test_session_name", ColourspaceType::kColourspaceRgb24, 640, 480, "127.0.0.1", 5004);
  ASSERT_EQ(rtp.GetHeight(), 640);
  ASSERT_EQ(rtp.GetWidth(), 480);
  ASSERT_EQ(rtp.GetColourSpace(), ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtp.GetIpAddress(), "127.0.0.1");
  ASSERT_EQ(rtp.GetPort(), 5004);
  rtp.Open();
  rtp.Transmit(rtb_test.data(), true);
  rtp.Close();
}
