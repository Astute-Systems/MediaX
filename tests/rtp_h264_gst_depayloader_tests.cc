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

#include "h264/gst/nvidia/rtp_h264_depayloader.h"
#include "h264/gst/vaapi/rtp_h264_depayloader.h"
#include "rtp/rtp_utils.h"
#include "uncompressed/rtp_uncompressed_payloader.h"
#include "util_tests.h"
#include "utils/colourspace_cpu.h"

TEST(RtpH264DepayloaderTest, Timeout) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  uint8_t* rgb_test;

  std::shared_ptr<mediax::rtp::RtpDepayloader> rtp;
  rtp = std::make_shared<mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader>();
  rtp->SetSessionName("test_session_name");
  ASSERT_EQ(rtp->GetSessionName(), "test_session_name");
  rtp->Open();
  EXPECT_FALSE(rtp->Receive(&rgb_test, 80));
  // Expect pointer to be invalid
  EXPECT_EQ(rgb_test, nullptr);
  rtp->Stop();
  rtp->Close();
}

TEST(RtpH264DepayloaderTest, GetSet) {
  mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader rtp;
  mediax::rtp::StreamInformation stream_info = {"test_session_name",
                                                "127.0.0.1",
                                                5004,
                                                640,
                                                480,
                                                25,
                                                mediax::rtp::ColourspaceType::kColourspaceH264Part10,
                                                false};
  rtp.SetStreamInfo(stream_info);

  ASSERT_EQ(rtp.GetHeight(), 640);
  ASSERT_EQ(rtp.GetWidth(), 480);
  ASSERT_EQ(rtp.GetColourSpace(), mediax::rtp::ColourspaceType::kColourspaceNv12);
  ASSERT_EQ(rtp.GetIpAddress(), "127.0.0.1");
  ASSERT_EQ(rtp.GetPort(), 5004);
  ASSERT_EQ(rtp.GetSessionName(), "test_session_name");
}

TEST(RtpH264DepayloaderTest, UnicastOk) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  std::array<uint8_t, 1280 * 720 * 3> rgb_test;
  mediax::video::ColourSpaceCpu colourspace;
  mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader rtp;

  // Set the stream details individually
  rtp.SetIpAddress("127.0.0.1");
  rtp.SetPort(5004);
  rtp.SetSessionName("test_session_name");
  rtp.SetHeight(720);
  rtp.SetWidth(1280);
  rtp.SetColourSpace(::mediax::rtp::ColourspaceType::kColourspaceH264Part10);

  // Start the stream
  EXPECT_TRUE(rtp.Open());
  rtp.Start();
  uint8_t* data = rgb_test.data();
  EXPECT_FALSE(rtp.Receive(&data, 80));
  rtp.Stop();
  rtp.Close();

  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceH264Part10);
  mediax::video::ColourSpaceCpu convert;
  convert.Nv12ToRgb(rtp.GetHeight(), rtp.GetWidth(), data, rgb_test.data());

  EXPECT_EQ(rtp.GetHeight(), 720);
  EXPECT_EQ(rtp.GetWidth(), 1280);

  WritePngFile(rgb_test.data(), rtp.GetWidth(), rtp.GetHeight(), "H264_Image.png");
}

TEST(RtpH264DepayloaderTest, UnicastOkSetStreamInfo) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  std::array<uint8_t, 1280 * 720 * 3> rgb_test;
  mediax::video::ColourSpaceCpu colourspace;
  mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader rtp;

  // Set the stream details using set stream info
  ::mediax::rtp::StreamInformation stream_info = {.session_name = "test_session_name",
                                                  .hostname = "127.0.0.1",
                                                  .port = 5004,
                                                  .height = 720,
                                                  .width = 1280,
                                                  .framerate = 25,
                                                  .encoding = ::mediax::rtp::ColourspaceType::kColourspaceH264Part10,
                                                  .deleted = false};
  rtp.SetStreamInfo(stream_info);

  // Start the stream
  EXPECT_TRUE(rtp.Open());
  rtp.Start();
  uint8_t* data = rgb_test.data();
  EXPECT_FALSE(rtp.Receive(&data, 80));
  rtp.Stop();
  rtp.Close();

  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
  mediax::video::ColourSpaceCpu convert;
  convert.Nv12ToRgb(rtp.GetHeight(), rtp.GetWidth(), data, rgb_test.data());

  EXPECT_EQ(rtp.GetHeight(), 720);
  EXPECT_EQ(rtp.GetWidth(), 1280);

  WritePngFile(rgb_test.data(), rtp.GetWidth(), rtp.GetHeight(), "H264_Image1.png");
}

TEST(RtpH264DepayloaderTest, UnicastOkSetStreamInfoPtr) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  std::array<uint8_t, 1280 * 720 * 3> rgb_test;
  mediax::video::ColourSpaceCpu colourspace;
  auto rtp = std::make_shared<mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader>();

  // Set the stream details using set stream info
  ::mediax::rtp::StreamInformation stream_info = {.session_name = "test_session_name",
                                                  .hostname = "127.0.0.1",
                                                  .port = 5004,
                                                  .height = 720,
                                                  .width = 1280,
                                                  .framerate = 25,
                                                  .encoding = ::mediax::rtp::ColourspaceType::kColourspaceH264Part10,
                                                  .deleted = false};
  rtp->SetStreamInfo(stream_info);

  // Start the stream
  EXPECT_TRUE(rtp->Open());
  rtp->Start();
  uint8_t* data = rgb_test.data();
  EXPECT_FALSE(rtp->Receive(&data, 80));
  rtp->Stop();
  rtp->Close();

  EXPECT_EQ(rtp->GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
  mediax::video::ColourSpaceCpu convert;
  convert.Nv12ToRgb(rtp->GetHeight(), rtp->GetWidth(), data, rgb_test.data());

  EXPECT_EQ(rtp->GetHeight(), 720);
  EXPECT_EQ(rtp->GetWidth(), 1280);

  WritePngFile(rgb_test.data(), rtp->GetWidth(), rtp->GetHeight(), "H264_Image2.png");
}

TEST(RtpH264DepayloaderTest, StateCheckNvidia) {
  mediax::rtp::h264::gst::nvidia::RtpH264GstNvidiaDepayloader rtp;
  mediax::rtp::StreamInformation stream_info = {"test_session_name",
                                                "127.0.0.1",
                                                5004,
                                                640,
                                                480,
                                                25,
                                                mediax::rtp::ColourspaceType::kColourspaceH264Part10,
                                                false};

  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kClosed);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
  rtp.SetStreamInfo(stream_info);
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kClosed);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
  rtp.Open();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kOpen);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
  rtp.Start();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kStarted);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
  rtp.Stop();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kStopped);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
  rtp.Close();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kClosed);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
}

TEST(RtpH264DepayloaderTest, StateCheckVappi) {
  mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader rtp;
  mediax::rtp::StreamInformation stream_info = {
      "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::rtp::ColourspaceType::kColourspaceRgb24, false};

  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kClosed);
  rtp.SetStreamInfo(stream_info);
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kClosed);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
  rtp.Open();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kOpen);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
  rtp.Start();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kStarted);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
  rtp.Stop();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kStopped);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
  rtp.Close();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kClosed);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceNv12);
}