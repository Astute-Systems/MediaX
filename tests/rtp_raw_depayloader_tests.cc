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

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include <chrono>
#include <thread>

#include "raw/rtpvraw_depayloader.h"
#include "raw/rtpvraw_payloader.h"
#include "rtp/rtp_utils.h"
#include "util_tests.h"
#include "utils/colourspace_cpu.h"

TEST(RtpRawDepayloaderTest, Copy) {
  mediax::RtpvrawDepayloader rtp;
  mediax::RtpvrawDepayloader rtp2;
  rtp = rtp2;
}

TEST(RtpRawDepayloaderTest, Timeout) {
  uint8_t* yuv_test;

  mediax::RtpvrawDepayloader rtp;
  mediax::StreamInformation stream_info = {
      "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::ColourspaceType::kColourspaceRgb24, false};
  rtp.SetStreamInfo(stream_info);
  ASSERT_EQ(rtp.GetHeight(), 640);
  ASSERT_EQ(rtp.GetWidth(), 480);
  ASSERT_EQ(rtp.GetColourSpace(), mediax::ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtp.GetIpAddress(), "127.0.0.1");
  ASSERT_EQ(rtp.GetPort(), 5004);
  ASSERT_EQ(rtp.GetSessionName(), "test_session_name");
  rtp.Open();
  EXPECT_FALSE(rtp.Receive(&yuv_test, 80));
  rtp.Close();
}

void SendVideoCheckered(std::string ip, uint32_t height, uint32_t width, uint32_t framerate, uint32_t portno) {
  mediax::RtpvrawPayloader rtp;
  mediax::StreamInformation stream_info = {
      "test_session_name", ip, portno, height, width, 25, mediax::ColourspaceType::kColourspaceRgb24, false};
  rtp.SetStreamInfo(stream_info);
  rtp.Open();
  rtp.Start();
  // Create a buffer of 640x480x3 bytes (RGB)
  std::vector<uint8_t> buffer(640 * 480 * 3);
  CreateCheckeredTestCard(buffer.data(), 640, 480, mediax::ColourspaceType::kColourspaceRgb24);
  rtp.Transmit(buffer.data(), 80);
  rtp.Stop();
  rtp.Close();
}

TEST(RtpRawDepayloaderTest, UnicastOk) {
  std::array<uint8_t, 640 * 480 * 3> rgb_test;
  mediax::video::ColourSpaceCpu colourspace;
  mediax::RtpvrawDepayloader rtp;
  mediax::StreamInformation stream_info = {
      "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::ColourspaceType::kColourspaceRgb24, false};
  rtp.SetStreamInfo(stream_info);
  rtp.Open();
  rtp.Start();
  SendVideoCheckered("127.0.0.1", 640, 480, 30, 5004);
  uint8_t* data = rgb_test.data();
  EXPECT_TRUE(rtp.Receive(&data, 80));
  WritePngFile(rgb_test.data(), 640, 480, "UnicastOk.png");
  rtp.Stop();
  rtp.Close();
}

void SendFrameThread(std::string ip, uint32_t port) {
  LOG(INFO) << "Sending video to " << ip << ":" << port;  // Sleep thread for 100ms
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  SendVideoCheckered(ip, 640, 480, 25, port);
  LOG(INFO) << "Sent " << ip << ":" << port;  // Sleep thread for 100ms
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// TEST(RtpRawDepayloaderTest, UnicastOkMany) {
//   std::array<uint8_t, 640 * 480 * 3> rgb_test;
//   uint8_t* data = rgb_test.data();
//   uint32_t base_port = 5004;
//   std::map<int, mediax::RtpvrawDepayloader> rtp;
//   int number_of_streams = 10;

//   // Setup ten streams
//   for (int i = 0; i < number_of_streams; i++) {
//     rtp[i].SetStreamInfo("test_session_name", mediax::ColourspaceType::kColourspaceRgb24, 640, 480, 25, "127.0.0.1",
//                          base_port + i);
//     rtp[i].Open();
//     rtp[i].Start();
//   }

//   // Send video to ten stream frames
//   for (int i = 0; i < number_of_streams; i++) {
//     // clear the buffer
//     memset(data, 0, 640 * 480 * 3);
//     // Start a transmit thread
//     // std::thread sender_thread(SendFrameThread, rtp[i].GetIpAddress(), rtp[i].GetPort());
//     SendVideoCheckered(rtp[i].GetIpAddress(), 640, 480, 25, rtp[i].GetPort());
//     EXPECT_TRUE(rtp[i].Receive(&data, 1000));
//     std::string filename = "UnicastOkMulti_" + std::to_string(i) + ".png";
//     WritePngFile(data, 640, 480, filename.c_str());
//   }

//   // Close the streams
//   for (int i = 0; i < number_of_streams; i++) {
//     rtp[i].Stop();
//     rtp[i].Close();
//   }
// }

TEST(RtpRawDepayloaderTest, MulticastOk) {
  std::array<uint8_t, 640 * 480 * 3> rgb_test;
  mediax::video::ColourSpaceCpu colourspace;

  mediax::RtpvrawDepayloader rtp;
  mediax::StreamInformation stream_info = {
      "test_session_name", "239.192.1.200", 5004, 640, 480, 25, mediax::ColourspaceType::kColourspaceRgb24, false};
  rtp.SetStreamInfo(stream_info);
  rtp.Open();
  rtp.Start();
  SendVideoCheckered("239.192.1.200", 640, 480, 30, 5004);
  uint8_t* data = rgb_test.data();
  EXPECT_TRUE(rtp.Receive(&data, 80));
  WritePngFile(rgb_test.data(), 640, 480, "MulticastOk.png");
  rtp.Stop();
  rtp.Close();
}

TEST(RtpRawDepayloaderTest, Many) {
  std::array<std::string, 10> ip_pool = {"239.192.1.1", "239.192.1.2", "239.192.1.3", "239.192.1.4", "239.192.1.5",
                                         "239.192.1.6", "239.192.1.7", "239.192.1.8", "239.192.1.9", "239.192.1.10"};
  std::array<mediax::RtpvrawDepayloader, 10> rtp;
  for (int i = 0; i < 10; i++) {
    LOG(INFO) << "Creating stream number " << i << " with IP:" << ip_pool[i];
    mediax::StreamInformation stream_info = {
        "test_session_name_" + std::to_string(i),    ip_pool[i], 5004, 640, 480, 25,
        mediax ::ColourspaceType::kColourspaceRgb24, false};
    rtp[i].SetStreamInfo(stream_info);
  }

  for (int i = 0; i < 10; i++) {
    LOG(INFO) << "Opening stream number " << rtp[i].GetIpAddress() << "\n";
    rtp[i].Open();
  }
}

TEST(RtpRawDepayloaderTest, ReOpening) {
  // Open stream 10 times
  for (int i = 0; i < 10; i++) {
    mediax::RtpvrawDepayloader rtp;
    mediax::StreamInformation stream_info = {
        "test_session_name", "127.0.0.1", 5004, 640, 480, 25, mediax::ColourspaceType::kColourspaceYuv, false};
    rtp.SetStreamInfo(stream_info);
    rtp.Open();
    rtp.Start();
    rtp.Stop();
    rtp.Close();
  }
}

void OpenStream(std::string ipaddr, uint32_t height, uint32_t width, uint32_t framerate, uint32_t portno) {
  std::vector<uint8_t> yuv_test;
  mediax::RtpvrawDepayloader rtp;
  yuv_test.resize(height * width * 2);
  mediax::StreamInformation stream_info = {"test", ipaddr, 5004, 640, 480, 25, mediax::ColourspaceType::kColourspaceYuv,
                                           false};
  rtp.SetStreamInfo(stream_info);
  rtp.Open();
  rtp.Start();

  bool running = true;
  int frame_count = 0;
  // Recieve video
  while (running) {
    uint8_t* data = yuv_test.data();
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
