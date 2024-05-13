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

#include "h264/gst/vaapi/rtp_h264_depayloader.h"
#include "h264/gst/vaapi/rtp_h264_payloader.h"
#include "rtp/rtp_utils.h"
#include "uncompressed/rtp_uncompressed_payloader.h"
#include "util_tests.h"
#include "utils/colourspace_cpu.h"

TEST(RtpH264VaapiDepayloaderTest, Timeout) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  mediax::rtp::RtpFrameData rgb_test;

  std::shared_ptr<mediax::rtp::RtpDepayloader> rtp;
  rtp = std::make_shared<mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader>();
  rtp->SetSessionName("test_session_name");
  ASSERT_EQ(rtp->GetSessionName(), "test_session_name");
  rtp->Open();
  EXPECT_FALSE(rtp->Receive(&rgb_test, 80));
  // Expect pointer to be invalid
  EXPECT_EQ(rgb_test.cpu_buffer, nullptr);
  rtp->Stop();
  rtp->Close();
}

TEST(RtpH264VaapiDepayloaderTest, GetSet) {
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
  ASSERT_EQ(rtp.GetColourSpace(), mediax::rtp::ColourspaceType::kColourspaceRgba);
  ASSERT_EQ(rtp.GetIpAddress(), "127.0.0.1");
  ASSERT_EQ(rtp.GetPort(), 5004);
  ASSERT_EQ(rtp.GetSessionName(), "test_session_name");
}

TEST(RtpH264VaapiDepayloaderTest, UnicastOk) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  std::array<uint8_t, 1280 * 720 * 4> rgb_test;
  mediax::video::ColourSpaceCpu colourspace;
  mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader rtp;

  // Set the stream details individually
  rtp.SetIpAddress("127.0.0.1");
  EXPECT_FALSE(rtp.SettingsValid());
  rtp.SetPort(5004);
  EXPECT_FALSE(rtp.SettingsValid());
  rtp.SetSessionName("test_session_name");
  EXPECT_FALSE(rtp.SettingsValid());
  rtp.SetHeight(720);
  EXPECT_FALSE(rtp.SettingsValid());
  rtp.SetWidth(1280);
  EXPECT_FALSE(rtp.SettingsValid());
  rtp.SetColourSpace(::mediax::rtp::ColourspaceType::kColourspaceH264Part10);
  EXPECT_FALSE(rtp.SettingsValid());
  rtp.SetFramerate(25);

  // When all the setting are manually set then the settings are valid
  EXPECT_TRUE(rtp.SettingsValid());

  // Start the stream
  EXPECT_TRUE(rtp.Open());
  rtp.Start();
  mediax::rtp::RtpFrameData data;
  data.cpu_buffer = rgb_test.data();
  EXPECT_FALSE(rtp.Receive(&data, 80));
  rtp.Stop();
  rtp.Close();

  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceH264Part10);
  mediax::video::ColourSpaceCpu convert;
  convert.RgbaToRgb(rtp.GetHeight(), rtp.GetWidth(), data.cpu_buffer, rgb_test.data());

  EXPECT_EQ(rtp.GetHeight(), 720);
  EXPECT_EQ(rtp.GetWidth(), 1280);

  WritePngFile(rgb_test.data(), rtp.GetWidth(), rtp.GetHeight(), "vaapiH264_Image.png");
}

TEST(RtpH264VaapiDepayloaderTest, UnicastOkSetStreamInfo) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  std::array<uint8_t, 1280 * 720 * 4> rgb_test;
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
  mediax::rtp::RtpFrameData data;
  data.cpu_buffer = rgb_test.data();
  EXPECT_FALSE(rtp.Receive(&data, 80));
  rtp.Stop();
  rtp.Close();

  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceRgba);
  mediax::video::ColourSpaceCpu convert;
  convert.RgbaToRgb(rtp.GetHeight(), rtp.GetWidth(), data.cpu_buffer, rgb_test.data());

  EXPECT_EQ(rtp.GetHeight(), 720);
  EXPECT_EQ(rtp.GetWidth(), 1280);

  WritePngFile(rgb_test.data(), rtp.GetWidth(), rtp.GetHeight(), "vaapiH264_Image1.png");
}

TEST(RtpH264VaapiDepayloaderTest, UnicastOkSetStreamInfoPtr) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  std::array<uint8_t, 1280 * 720 * 4> rgb_test;
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
  mediax::rtp::RtpFrameData data;
  data.cpu_buffer = rgb_test.data();
  EXPECT_FALSE(rtp->Receive(&data, 80));
  rtp->Stop();
  rtp->Close();

  EXPECT_EQ(rtp->GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceRgba);
  mediax::video::ColourSpaceCpu convert;
  convert.RgbaToRgb(rtp->GetHeight(), rtp->GetWidth(), data.cpu_buffer, rgb_test.data());

  EXPECT_EQ(rtp->GetHeight(), 720);
  EXPECT_EQ(rtp->GetWidth(), 1280);

  WritePngFile(rgb_test.data(), rtp->GetWidth(), rtp->GetHeight(), "vaapiH264_Image2.png");
}

TEST(RtpH264VaapiDepayloaderTest, kColourspaceRgba) {
  mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader rtp;
  mediax::rtp::StreamInformation stream_info = {
      "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::rtp::ColourspaceType::kColourspaceRgb24, false};

  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceRgba);
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kClosed);
  rtp.SetStreamInfo(stream_info);
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kClosed);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceRgba);
  rtp.Open();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kOpen);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceRgba);
  rtp.Start();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kStarted);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceRgba);
  rtp.Stop();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kStopped);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceRgba);
  rtp.Close();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kClosed);
  EXPECT_EQ(rtp.GetColourSpace(), ::mediax::rtp::ColourspaceType::kColourspaceRgba);
}

TEST(RtpH264VaapiDepayloaderTest, FailToVaapi) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  std::array<uint8_t, 1280 * 720 * 4> rgb_test;
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

  rtp.Stop();
  rtp.Close();
}

TEST(RtpH264VaapiDepayloaderTest, TestNoStreamSet) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  std::array<uint8_t, 1280 * 720 * 4> rgb_test;
  mediax::video::ColourSpaceCpu colourspace;
  mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader rtp;

  // Dont set the stream details, will error

  // Start the stream
  EXPECT_FALSE(rtp.Open());
  rtp.Start();
  mediax::rtp::RtpFrameData data;
  data.cpu_buffer = rgb_test.data();
  EXPECT_FALSE(rtp.Receive(&data, 80));
  rtp.Stop();
  rtp.Close();
}

void Stream(mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader* rtp, std::string name, std::string ip,
            uint16_t port) {
  // Set the stream details using set stream info
  ::mediax::rtp::StreamInformation stream_info = {.session_name = name,
                                                  .hostname = ip,
                                                  .port = port,
                                                  .height = 720,
                                                  .width = 1280,
                                                  .framerate = 25,
                                                  .encoding = ::mediax::rtp::ColourspaceType::kColourspaceH264Part10,
                                                  .deleted = false};
  rtp->SetStreamInfo(stream_info);
  // Start the stream
  EXPECT_TRUE(rtp->Open());
}

TEST(RtpH264VaapiDepayloaderTest, StartSwitchManyPayloaders) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  int last_stream = -1;
  int current_stream = -1;

  std::array<uint8_t, 1280 * 720 * 4> rgb_test;
  mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader rtp[5];

  // Vaapi five streams
  for (int i = 0; i < 5; i++) {
    std::string name = "test_session_name_" + std::to_string(i);
    std::string ip = "239.192.1." + std::to_string(i);
    Stream(&rtp[i], name, ip, 5004);
  }

  // Randomly switch active streams 100 times
  for (int i = 0; i < 10; i++) {
    if (last_stream >= 0) {
      rtp[last_stream].Stop();
    }
    current_stream = rand() % 5;
    rtp[current_stream].Start();
    // Transmit a frame
    mediax::rtp::RtpFrameData data;
    data.cpu_buffer = rgb_test.data();
    EXPECT_FALSE(rtp[current_stream].Receive(&data, 1));
    EXPECT_EQ(data.resolution.height, 0);
    EXPECT_EQ(data.resolution.width, 0);
    last_stream = current_stream;
  }
  rtp[current_stream].Stop();

  // Stop and close
  for (int i = 0; i < 5; i++) {
    rtp[i].Close();
  }
}

TEST(RtpH264VaapiDepayloaderTest, TransmitAFrame) {
#if !GST_SUPPORTED
  GTEST_SKIP();
#endif

  // Read CI environment variable
  char* ci = getenv("CI");

  // If defined skip test, doesnt run on CI
  if (ci != NULL) {
    GTEST_SKIP();
  }

  int last_stream = -1;
  int current_stream = -1;

  std::array<uint8_t, 1280 * 720 * 4> rgb_test;
  mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader rtp;

  std::string name = "test_session_name";
  std::string ip = "239.192.1.1";
  Stream(&rtp, name, ip, 5004);
  rtp.Start();

  // Setup receiver
  mediax::rtp::RtpFrameData data;
  data.cpu_buffer = rgb_test.data();

  // Transmit a frame
  mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiPayloader rtp_pay;
  rtp_pay.SetIpAddress("test_session_name");
  rtp_pay.SetHeight(720);
  rtp_pay.SetWidth(1280);
  rtp_pay.SetIpAddress(ip);
  rtp_pay.SetPort(5004);
  rtp_pay.SetFrameRate(25);
  rtp_pay.SetColourSpace(::mediax::rtp::ColourspaceType::kColourspaceH264Part10);
  rtp_pay.Open();
  rtp_pay.Start();

  for (int i = 0; i < 3; i++) {
    rtp_pay.Transmit(rgb_test.data(), 80);
  }
  // Sleep 0.5 seconds
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  EXPECT_TRUE(rtp.Receive(&data, 5000));
  EXPECT_EQ(data.resolution.height, 720);
  EXPECT_EQ(data.resolution.width, 1280);
  EXPECT_EQ(data.encoding, ::mediax::rtp::ColourspaceType::kColourspaceRgb24);

  rtp.Stop();
  rtp.Close();

  rtp_pay.Stop();
  rtp_pay.Close();
}
