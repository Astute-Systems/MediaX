//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
//
//
// Example RTP packet from wireshark
//      Real-Time Transport Protocol
//      10.. .... = Version: RFC 1889 Version (2)
//      ..0. .... = Padding: False
//      ...0 .... = Extension: False
//      .... 0000 = Contributing source identifiers count: 0
//      0... .... = Marker: False
//      Payload type: DynamicRTP-Type-96 (96)
//      Sequence number: 34513
//      Timestamp: 2999318601
//      Synchronization Source identifier: 0xdccae7a8 (3704285096)
//      Payload: 000003c000a08000019e00a2000029292929f06e29292929...
//
// Gstreamer1 .0 working example UYVY streaming
// ============================================
//
// gst-launch-1.0 videotestsrc num_buffers ! video/x-raw,
//    format=UYVY, framerate=25/1, width=640,
//    height=480 ! queue ! rtpvrawpay ! udpsink hos=127.0.0.1 port=5004
//
// gst-launch-1.0 udpsrc port=5004 caps="application/x-rtp, media=(string)video, clock-rate=(int)90000,
//    encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:2, depth=(string)8, width=(string)480,
//    height=(string)480, payload=(int)96" ! queue ! rtpvrawdepay ! queue ! xvimagesink sync=false
//
// Use his program to stream data to the udpsc example above on the Jetson
//
/// \file transmit-example.cc

#include <byteswap.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <array>
#include <climits>
#include <csignal>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

#include "example.h"
#include "example_helpers.h"
#include "pngget.h"
#include "rtp/rtp.h"
#include "v4l2/v4l2_source.h"
#include "version.h"

DEFINE_string(ipaddr, kIpAddressDefault, "the IP address of the transmit stream");
DEFINE_int32(port, kPortDefault, "the port to use for the transmit stream");
DEFINE_int32(height, kHeightDefault, "the height of the image");
DEFINE_int32(width, kWidthDefault, "the width of the image");
DEFINE_int32(source, 2,
             "The video source (0-10)\n\t"
             "0 - Use a PNG file (see -filename)\n\t"
             "1 - v4l2src\n\t"
             "2 - Colour bars\n\t"
             "3 - Greyscale bars\n\t"
             "4 - Scaled RGB values\n\t"
             "5 - Checkered test card\n\t"
             "6 - Solid white\n\t"
             "7 - Solid black\n\t"
             "8 - Solid red\n\t"
             "9 - Solid green\n\t"
             "10 - Solid blue\n\t");
DEFINE_string(filename, "testcard.png", "the PNG file to use as the source of the video stream (only with -source 0)");
DEFINE_string(device, "/dev/video0", "the V4L2 device source (only with -source 1)");
DEFINE_string(session_name, "TestVideo1", "the SAP/SDP session name");
DEFINE_int32(mode, 0,
             "The video mode (0-4)\n\t"
             "0 - Uncompressed YUV\n\t"
             "1 - Uncompressed RGB\n\t"
             "2 - Mono16\n\t"
             "3 - Mono8\n\t"
             "4 - H.264\n\t");

static bool application_running = true;

void signalHandler(int signum [[maybe_unused]]) { application_running = false; }

int main(int argc, char **argv) {
#if CUDA_ENABLED
  std::shared_ptr<video::ColourSpace> convert = std::make_shared<video::ColourSpaceCuda>();
#else
  std::shared_ptr<video::ColourSpace> convert = std::make_shared<video::ColourSpaceCpu>();
#endif
  mediax::ColourspaceType video_mode = mediax::ColourspaceType::kColourspaceYuv;

  std::unique_ptr<V4L2Capture> v4lsource;

  gflags::SetVersionString(kVersion);
  gflags::SetUsageMessage(
      "Transmit a video stream using RTP\n"
      "Usage:\n"
      "    transmit-example [options]\n"
      "Example:\n"
      "    transmit-example -ipaddr 127.0.0.1 -port 5004 -height 480 -width 640 -source 0\n");
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  mediax::RtpInit(argc, argv);

  uint32_t frame = 0;
  const uint32_t kBuffSize = FLAGS_height * FLAGS_width;
  const uint32_t kBuffSizeRGB = kBuffSize * 3;
  std::vector<uint8_t> transmit_buffer;  // Maximum is HD

  // register signal SIGINT and signal handler
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  std::cout << "Example RTP streaming (" << FLAGS_width << "x" << FLAGS_height << " " << ModeToString(FLAGS_mode)
            << ") to " << FLAGS_ipaddr.c_str() << ":" << FLAGS_port << "\n";

  video_mode = GetMode(FLAGS_mode);
  transmit_buffer.resize(FLAGS_height * FLAGS_width * BytesPerPixel(video_mode));

  // Setup RTP streaming class
  mediax::RtpvrawPayloader rtp;
  rtp.SetStreamInfo(FLAGS_filename, video_mode, FLAGS_height, FLAGS_width, FLAGS_ipaddr, (uint16_t)FLAGS_port);
  rtp.Open();

  // Read the PNG file
  std::vector<uint8_t> rgb;
  switch (FLAGS_source) {
    case 1:
      rgb.resize(kBuffSizeRGB);
      v4lsource = std::make_unique<V4L2Capture>(FLAGS_device, FLAGS_height, FLAGS_width);
      if (v4lsource->Initalise() != 0) {
        LOG(ERROR) << "Failed to initalise V4l2 device";
        return 1;
      }
      v4lsource->CaptureFrame(rgb.data());
      LOG(INFO) << "Creating V4l2 source device=" + FLAGS_device;
      break;
    case 2:
      rgb.resize(kBuffSizeRGB);
      CreateColourBarTestCard(rgb.data(), FLAGS_width, FLAGS_height, video_mode);
      LOG(INFO) << "Creating colour bar test card";
      break;
    case 3:
      rgb.resize(kBuffSizeRGB);
      CreateGreyScaleBarTestCard(rgb.data(), FLAGS_width, FLAGS_height, video_mode);
      LOG(INFO) << "Creating greyscale test card";
      break;
    case 4:
      rgb.resize(kBuffSizeRGB);
      CreateComplexTestCard(rgb.data(), FLAGS_width, FLAGS_height, video_mode);
      LOG(INFO) << "Creating scaled RGB values";
      break;
    case 5:
      rgb.resize(kBuffSizeRGB);
      CreateCheckeredTestCard(rgb.data(), FLAGS_width, FLAGS_height, video_mode);
      LOG(INFO) << "Creating checkered test card";
      break;
    case 6:  // Solid white
      rgb.resize(kBuffSizeRGB);
      CreateSolidTestCard(rgb.data(), FLAGS_width, FLAGS_height, 255, 255, 255, video_mode);
      LOG(INFO) << "Creating solid white test card";
      break;
    case 7:  // Solid black
      rgb.resize(kBuffSizeRGB);
      CreateSolidTestCard(rgb.data(), FLAGS_width, FLAGS_height, 0, 0, 0, video_mode);
      LOG(INFO) << "Creating solid black test card";
      break;
    case 8:  // Solid red
      rgb.resize(kBuffSizeRGB);
      CreateSolidTestCard(rgb.data(), FLAGS_width, FLAGS_height, 255, 0, 0, video_mode);
      LOG(INFO) << "Creating solid red test card";
      break;
    case 9:  // Solid green
      rgb.resize(kBuffSizeRGB);
      CreateSolidTestCard(rgb.data(), FLAGS_width, FLAGS_height, 0, 255, 0, video_mode);
      LOG(INFO) << "Creating solid green test card";
      break;
    case 10:  // Solid blue
      rgb.resize(kBuffSizeRGB);
      CreateSolidTestCard(rgb.data(), FLAGS_width, FLAGS_height, 0, 0, 255, video_mode);
      LOG(INFO) << "Creating solid blue test card";
      break;
    case 0:
      Png image_reader;
      rgb = image_reader.ReadPngRgb24(FLAGS_filename);
      if (rgb.empty()) {
        LOG(INFO) << "Failed to read png file (" << FLAGS_filename << ")";
        return -1;
      }
      /// Make it RGB
      convert->RgbaToRgb(FLAGS_height, FLAGS_width, rgb.data(), rgb.data());
      break;
    default:
      LOG(INFO) << "Invalid source mode (" << FLAGS_source << ")";
      break;
  }

  // Convert all the scan lines
  // Loop frames forever
  while (application_running) {
    // Timestamp start
    auto start = std::chrono::high_resolution_clock::now();
    // Clear the YUV buffer
    if (FLAGS_source == 1) {
      v4lsource->CaptureFrame(rgb.data());
    }

    // Clear buffer
    memset(transmit_buffer.data(), 0, kBuffSize);
    // Copy new image into buffer
    memcpy(transmit_buffer.data(), rgb.data(), FLAGS_height * FLAGS_width * BytesPerPixel(video_mode));

    if (rtp.Transmit(transmit_buffer.data(), true) < 0) break;
    auto end = std::chrono::high_resolution_clock::now();

    if (auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); duration < 40) {
      std::this_thread::sleep_for(std::chrono::milliseconds(40 - duration));
    }

    frame++;
    std::cout << "Frame: " << frame << "\r" << std::flush;
  }
  std::cout << "\n";
  std::cout << "Example terminated...\n";

  rtp.Close();
  mediax::RtpCleanup();

  return 0;
}
