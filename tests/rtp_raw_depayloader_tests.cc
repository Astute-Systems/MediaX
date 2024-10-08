//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \file rtp_raw_depayloader_tests.cc
///

#include <gtest/gtest.h>
#include <unistd.h>

#include <chrono>
#include <memory>
#include <thread>

#include "mock_rtp_depayloader.h"
#include "mock_rtp_payloader.h"
#include "rtp/rtp_types.h"
#include "rtp/rtp_utils.h"
#include "uncompressed/rtp_uncompressed_depayloader.h"
#include "uncompressed/rtp_uncompressed_payloader.h"
#include "util_tests.h"
#include "utils/colourspace_cpu.h"

TEST(RtpRawDepayloaderTest, Buffer) {
  mediax::rtp::uncompressed::RtpUncompressedDepayloader rtp;
  rtp.SetBufferSize(1000);
  ASSERT_EQ(rtp.GetBufferSize(), 1000);
  // fill with 0
  memset(rtp.GetBuffer().data(), 0, 1000);
}

TEST(RtpRawDepayloaderTest, Timeout) {
  mediax::rtp::RtpFrameData yuv_test;
  mediax::rtp::uncompressed::RtpUncompressedDepayloader rtp;

  mediax::rtp::StreamInformation stream_info = {
      "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::rtp::ColourspaceType::kColourspaceRgb24, false};
  rtp.SetStreamInfo(stream_info);

  ASSERT_EQ(rtp.GetHeight(), 640);
  ASSERT_EQ(rtp.GetWidth(), 480);
  ASSERT_EQ(rtp.GetColourSpace(), mediax::rtp::ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtp.GetIpAddress(), "127.0.0.1");
  ASSERT_EQ(rtp.GetPort(), 5004);
  ASSERT_EQ(rtp.GetSessionName(), "test_session_name");
  rtp.Open();
  rtp.Start();

  EXPECT_FALSE(rtp.Receive(&yuv_test, 80));
  EXPECT_NE(yuv_test.cpu_buffer, nullptr);
  rtp.Stop();
  rtp.Close();
}

void SendVideoCheckered(std::string ip, uint32_t height, uint32_t width, uint32_t framerate, uint32_t portno) {
  mediax::rtp::MockRtpPayloader rtp;
  EXPECT_CALL(rtp, SetStreamInfo);
  EXPECT_CALL(rtp, Open);
  EXPECT_CALL(rtp, Start);
  EXPECT_CALL(rtp, Stop);
  EXPECT_CALL(rtp, Close);
  EXPECT_CALL(rtp, Transmit);

  mediax::rtp::StreamInformation stream_info = {
      "test_session_name", ip, portno, height, width, 25, mediax::rtp::ColourspaceType::kColourspaceRgb24, false};
  rtp.SetStreamInfo(stream_info);
  rtp.Open();
  rtp.Start();
  // Create a buffer of 640x480x3 bytes (RGB)
  std::vector<uint8_t> buffer(640 * 480 * 3);
  CreateCheckeredTestCard(buffer.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  rtp.Transmit(buffer.data(), 80);
  rtp.Stop();
  rtp.Close();
}

TEST(RtpRawDepayloaderTest, UnicastOk) {
  std::array<uint8_t, 640 * 480 * 3> rgb_test;
  mediax::video::ColourSpaceCpu colourspace;
  mediax::rtp::MockRtpDepayloader rtp;
  EXPECT_CALL(rtp, SetStreamInfo);
  EXPECT_CALL(rtp, Open);
  EXPECT_CALL(rtp, Start);
  EXPECT_CALL(rtp, Receive).WillOnce(testing::Return(true));
  EXPECT_CALL(rtp, Stop);
  EXPECT_CALL(rtp, Close);

  mediax::rtp::StreamInformation stream_info = {
      "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::rtp::ColourspaceType::kColourspaceRgb24, false};
  rtp.SetStreamInfo(stream_info);
  rtp.Open();
  rtp.Start();

  mediax::rtp::RtpFrameData data;
  data.cpu_buffer = rgb_test.data();
  EXPECT_TRUE(rtp.Receive(&data, 80));
  rtp.Stop();
  rtp.Close();
}

void SendFrameThread(std::string ip, uint32_t port) {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  SendVideoCheckered(ip, 640, 480, 25, port);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(RtpRawDepayloaderTest, Many) {
  std::array<std::string, 10> ip_pool = {"239.192.1.1", "239.192.1.2", "239.192.1.3", "239.192.1.4", "239.192.1.5",
                                         "239.192.1.6", "239.192.1.7", "239.192.1.8", "239.192.1.9", "239.192.1.10"};
  std::array<mediax::rtp::uncompressed::RtpUncompressedDepayloader, 10> rtp;
  for (int i = 0; i < 10; i++) {
    mediax::rtp::StreamInformation stream_info = {
        "test_session_name_" + std::to_string(i),         ip_pool[i], 5004, 640, 480, 25,
        mediax ::rtp::ColourspaceType::kColourspaceRgb24, false};
    rtp[i].SetStreamInfo(stream_info);
  }

  // Open and Start
  for (int i = 0; i < 10; i++) {
    rtp[i].Open();
    rtp[i].Start();
  }

  // Stop and Close
  for (int i = 0; i < 10; i++) {
    rtp[i].Stop();
    rtp[i].Close();
  }
}

TEST(RtpRawDepayloaderTest, ReOpening) {
  // Open stream 10 times
  for (int i = 0; i < 10; i++) {
    mediax::rtp::uncompressed::RtpUncompressedDepayloader rtp;
    mediax::rtp::StreamInformation stream_info = {
        "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::rtp::ColourspaceType::kColourspaceYuv422, false};
    rtp.SetStreamInfo(stream_info);
    rtp.Open();
    rtp.Start();
    rtp.Stop();
    rtp.Close();
  }
}

void OpenStream(std::string ipaddr, uint32_t height, uint32_t width, uint32_t framerate, uint32_t portno) {
  std::vector<uint8_t> yuv_test;
  mediax::rtp::uncompressed::RtpUncompressedDepayloader rtp;
  yuv_test.resize(height * width * 2);
  mediax::rtp::StreamInformation stream_info = {
      "test", ipaddr, 5004, 640, 480, 25, mediax::rtp::ColourspaceType::kColourspaceYuv422, false};
  rtp.SetStreamInfo(stream_info);
  rtp.Open();
  rtp.Start();

  bool running = true;
  int frame_count = 0;
  // Recieve video
  while (running) {
    mediax::rtp::RtpFrameData data;
    data.cpu_buffer = yuv_test.data();
    bool ret = rtp.Receive(&data, 0);
    EXPECT_TRUE(ret);
    if (ret == false) break;
    if (frame_count++ > 10) running = false;
  }

  rtp.Stop();
  rtp.Close();
}

std::string ExecuteCommand(std::string bash) {
  std::string result = "";
  FILE* pipe = popen(bash.c_str(), "r");
  if (!pipe) throw std::runtime_error("popen() failed!");
  try {
    while (!feof(pipe)) {
      char buf[256];
      if (fgets(buf, 256, pipe) != nullptr) result += buf;
    }
  } catch (...) {
    pclose(pipe);
    throw;
  }
  pclose(pipe);
  return result;
}

TEST(RtpRawDepayloaderTest, DISABLED_SwitchStreams) {
  // Start five streams run the bash script start_five_streams.sh
  ExecuteCommand("../scripts/start_five_streams.sh");
  // Switch between 5 streams
  std::cout << "Opening stream 1" << std::endl;
  OpenStream("239.192.3.1", 640, 480, 30, 5004);
  std::cout << "Opening stream 2" << std::endl;
  OpenStream("239.192.3.2", 640, 480, 30, 5004);
  std::cout << "Opening stream 3" << std::endl;
  OpenStream("239.192.3.3", 640, 480, 30, 5004);
  std::cout << "Opening stream 4" << std::endl;
  OpenStream("239.192.3.4", 640, 480, 30, 5004);
  std::cout << "Opening stream 5" << std::endl;
  OpenStream("239.192.3.5", 640, 480, 30, 5004);
  ExecuteCommand("pkill -f gst-launch-1.0");
}

std::array<uint8_t, 640 * 480 * 3> rgb_test;
bool done = false;

static ::mediax::rtp::RtpCallback callback_;

void StoreCallback(const ::mediax::rtp::RtpCallback& callback) { callback_ = callback; }

void CallCallback() {
  ::mediax::rtp::RtpFrameData frame = {};
  ::mediax::rtp::uncompressed::RtpUncompressedDepayloader depay;
  callback_(static_cast<const ::mediax::rtp::RtpDepayloader&>(depay), frame);
}

TEST(RtpRawDepayloaderTest, Callback) {
  mediax::video::ColourSpaceCpu colourspace;

  mediax::rtp::MockRtpDepayloader rtp;
  EXPECT_CALL(rtp, SetStreamInfo);
  EXPECT_CALL(rtp, Open);
  EXPECT_CALL(rtp, Start);
  EXPECT_CALL(rtp, Receive).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(rtp, Stop);
  EXPECT_CALL(rtp, Close);
  EXPECT_CALL(rtp, RegisterCallback).WillRepeatedly(testing::Invoke(StoreCallback));
  EXPECT_CALL(rtp, Callback).WillRepeatedly(testing::Invoke(CallCallback));

  mediax::rtp::StreamInformation stream_info = {
      "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::rtp::ColourspaceType::kColourspaceRgb24, false};
  rtp.SetStreamInfo(stream_info);

  // Recieve the frame via a callback
  rtp.RegisterCallback([](const mediax::rtp::RtpDepayloader& rtp, mediax::rtp::RtpFrameData frame) {
    memcpy(rgb_test.data(), frame.cpu_buffer, rtp.GetWidth() * rtp.GetHeight() * 3);
    std::cout << "Callback called" << std::endl;
    done = true;
  });
  rtp.Open();
  rtp.Start();
  SendVideoCheckered("127.0.0.1", 640, 480, 25, 5004);

#if 0
  uint8_t* data = rgb_test.data();
  EXPECT_TRUE(rtp.Receive(&data, 80));
#else
  ::mediax::rtp::RtpFrameData frame = {};
  rtp.Callback(frame);
  while (!done) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Waiting for callback" << std::endl;
  }
#endif

  WritePngFile(rgb_test.data(), 640, 480, "MulticastOkCallback.png");
  rtp.Stop();
  rtp.Close();
}

TEST(RtpRawDepayloaderTest, StateCheck) {
  mediax::rtp::uncompressed::RtpUncompressedDepayloader rtp;
  mediax::rtp::StreamInformation stream_info = {
      "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::rtp::ColourspaceType::kColourspaceRgb24, false};

  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kClosed);
  rtp.SetStreamInfo(stream_info);
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kClosed);
  rtp.Open();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kOpen);
  rtp.Start();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kStarted);
  rtp.Stop();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kStopped);
  rtp.Close();
  EXPECT_EQ(rtp.GetState(), ::mediax::rtp::StreamState::kClosed);
}