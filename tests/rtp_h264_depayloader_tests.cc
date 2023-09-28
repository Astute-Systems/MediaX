//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \file rtp_h264_depayloader_tests.cc
///

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include <chrono>
#include <thread>

#include "h264/gst/rtph264_depayloader.h"
#include "raw/rtpvraw_payloader.h"
#include "rtp/rtp_utils.h"
#include "util_tests.h"
#include "utils/colourspace_cpu.h"

TEST(RtpH264DepayloaderTest, DISABLED_Timeout) {
  uint8_t* rgb_test;

  mediax::RtpH264Depayloader rtp;
  rtp.SetSessionName("test_session_name");
  ASSERT_EQ(rtp.GetSessionName(), "test_session_name");
  rtp.Open();
  EXPECT_FALSE(rtp.Receive(&rgb_test, 80));
  rtp.Close();
}

TEST(RtpH264DepayloaderTest, DISABLED_UnicastOk) {
  std::array<uint8_t, 640 * 480 * 3> rgb_test;
  video::ColourSpaceCpu colourspace;
  mediax::RtpH264Depayloader rtp;

  // Increase GST debug level
  setenv("GST_DEBUG", "3", 1);
  // Set the stream details
  rtp.SetIpAddress("127.0.0.1");
  rtp.SetPort(5004);
  std::cout << "Setup" << std::endl;

  // Start the stream
  rtp.Open();
  std::cout << "Opened" << std::endl;
  rtp.Start();
  std::cout << "Started" << std::endl;
  uint8_t* data = rgb_test.data();
  EXPECT_TRUE(rtp.Receive(&data, 800));
  std::cout << "Received" << std::endl;
  WritePngFile(data, 1280, 720, "H264_Image.png");
  rtp.Stop();
  rtp.Close();
}
