//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \file rtp_jpeg_depayloader_tests.cc
///

#include <gtest/gtest.h>
#include <unistd.h>

#include <chrono>
#include <thread>

#include "jpeg/gst/rtp_jpeg_depayloader.h"
#include "jpeg/gst/rtp_jpeg_payloader.h"
#include "rtp/rtp_utils.h"
#include "util_tests.h"
#include "utils/colourspace_cpu.h"

TEST(RtpJpegDepayloaderTest, UnicastOkSetStreamInfo) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  std::array<uint8_t, 1280 * 720 * 3> rgb_test;
  mediax::video::ColourSpaceCpu colourspace;
  mediax::rtp::jpeg::gst::RtpJpegGstDepayloader rtp;

  // Set the stream details using set stream info
  ::mediax::rtp::StreamInformation stream_info = {.session_name = "test_session_jpeg_name",
                                                  .hostname = "127.0.0.1",
                                                  .port = 5004,
                                                  .height = 720,
                                                  .width = 1280,
                                                  .framerate = 25,
                                                  .encoding = ::mediax::rtp::ColourspaceType::kColourspaceJpeg2000,
                                                  .deleted = false};
  rtp.SetStreamInfo(stream_info);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceRgb24);

  // Start the stream
  EXPECT_TRUE(rtp.Open());
  rtp.Start();
  mediax::rtp::RtpFrameData data;
  data.cpu_buffer = rgb_test.data();
  EXPECT_FALSE(rtp.Receive(&data, 2000));
  rtp.Stop();
  rtp.Close();

  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceRgb24);
  EXPECT_EQ(rtp.GetHeight(), 720);
  EXPECT_EQ(rtp.GetWidth(), 1280);

  memcpy(rgb_test.data(), data.cpu_buffer, 1280 * 720 * 3);
  WritePngFile(rgb_test.data(), rtp.GetWidth(), rtp.GetHeight(), "JPEG_Image1.png");
}
