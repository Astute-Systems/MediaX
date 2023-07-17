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
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <array>
#include <csignal>
#include <iostream>
#include <ostream>
#include <vector>

#include "colourspace.h"
#include "colourspace_cuda.h"
#include "example.h"
#include "pngget.h"
#include "rtp_utils.h"
#include "rtpvraw_payloader.h"
#include "v4l2_source.h"
#include "version.h"

DEFINE_string(ipaddr, kIpAddressDefault, "the IP address of the transmit stream");
DEFINE_int32(port, kPortDefault, "the port to use for the transmit stream");
DEFINE_int32(height, kHeightDefault, "the height of the image");
DEFINE_int32(width, kWidthDefault, "the width of the image");
DEFINE_int32(source, 0,
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

static bool application_running = true;

void signalHandler(int signum [[maybe_unused]]) { application_running = false; }

int main(int argc, char **argv) {
  video::ColourSpace convert;

  gflags::SetVersionString(kVersion);
  gflags::SetUsageMessage(
      "Transmit a video stream using RTP\n"
      "Usage:\n"
      "    transmit-example [options]\n"
      "Example:\n"
      "    transmit-example -ipaddr 127.0.0.1 -port 5004 -height 480 -width 640 -source 0\n");
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  uint32_t frame = 0;
  const uint32_t kBuffSize = FLAGS_height * FLAGS_width;
  const uint32_t kBuffSizeRGB = kBuffSize * 3;
  std::array<uint8_t, (640 * 480) * 2> yuv;       // Maximum is HD
  std::array<uint8_t, (640 * 480) * 3> rtb_test;  // Maximum is HD

  // register signal SIGINT and signal handler
  signal(SIGINT, signalHandler);

  std::cout << "Example RTP streaming (" << FLAGS_width << "x" << FLAGS_height << ") to " << FLAGS_ipaddr.c_str() << ":"
            << FLAGS_port << "\n";

  sleep(1);

  // Setup RTP streaming class
  RtpvrawPayloader rtp;
  rtp.SetStreamInfo(FLAGS_filename, ColourspaceType::kColourspaceYuv, FLAGS_height, FLAGS_width, FLAGS_ipaddr,
                    (uint16_t)FLAGS_port);
  rtp.Open();

  memset(rtb_test.data(), 0, kBuffSize * 3);

  // Read the PNG file
  std::vector<uint8_t> rgb;
  switch (FLAGS_source) {
    case 1:
      rgb.resize(kBuffSizeRGB);
      {
        auto v4lsource = V4L2Capture(FLAGS_device, FLAGS_width, FLAGS_height);
        // v4lsource.CaptureFrame(rgb.data());
      }
      std::cout << "Creating V4l2 source device=" + FLAGS_device + "\n";
      break;
    case 2:
      rgb.resize(kBuffSizeRGB);
      CreateColourBarTestCard(rgb.data(), FLAGS_width, FLAGS_height);
      std::cout << "Creating colour bar test card\n";
      break;
    case 3:
      rgb.resize(kBuffSizeRGB);
      CreateGreyScaleBarTestCard(rgb.data(), FLAGS_width, FLAGS_height);
      std::cout << "Creating greyscale test card\n";
      break;
    case 4:
      rgb.resize(kBuffSizeRGB);
      CreateComplexTestCard(rgb.data(), FLAGS_width, FLAGS_height);
      std::cout << "Creating scaled RGB values\n";
      break;
    case 5:
      rgb.resize(kBuffSizeRGB);
      CreateCheckeredTestCard(rgb.data(), FLAGS_width, FLAGS_height);
      std::cout << "Creating checkered test card\n";
      break;
    case 6:  // Solid white
      rgb.resize(kBuffSizeRGB);
      CreateSolidTestCard(rgb.data(), FLAGS_width, FLAGS_height, 255, 255, 255);
      std::cout << "Creating solid white test card\n";
      break;
    case 7:  // Solid black
      rgb.resize(kBuffSizeRGB);
      CreateSolidTestCard(rgb.data(), FLAGS_width, FLAGS_height, 0, 0, 0);
      std::cout << "Creating solid black test card\n";
      break;
    case 8:  // Solid red
      rgb.resize(kBuffSizeRGB);
      CreateSolidTestCard(rgb.data(), FLAGS_width, FLAGS_height, 255, 0, 0);
      std::cout << "Creating solid red test card\n";
      break;
    case 9:  // Solid green
      rgb.resize(kBuffSizeRGB);
      CreateSolidTestCard(rgb.data(), FLAGS_width, FLAGS_height, 0, 255, 0);
      std::cout << "Creating solid green test card\n";
      break;
    case 10:  // Solid blue
      rgb.resize(kBuffSizeRGB);
      CreateSolidTestCard(rgb.data(), FLAGS_width, FLAGS_height, 0, 0, 255);
      std::cout << "Creating solid blue test card\n";
      break;
    default:
      Png image_reader;
      rgb = image_reader.ReadPngRgb24(FLAGS_filename);
      if (rgb.empty()) {
        std::cout << "Failed to read png file (" << FLAGS_filename << ")";
        return -1;
      }
      /// Make it RGB
      convert.RgbaToRgb(FLAGS_height, FLAGS_width, rgb.data(), rgb.data());
      break;
  }

  // Convert all the scan lines
  // Loop frames forever
  while (application_running) {
    // Timestamp start
    auto start = std::chrono::high_resolution_clock::now();
    // Clear the YUV buffer
    // memset(yuv.data(), 0, kBuffSize);

    // Convert the RGB data to YUV again
    convert.RgbToYuv(FLAGS_height, FLAGS_width, rgb.data(), yuv.data());
    // video::cuda::RgbaToYuv(FLAGS_height, FLAGS_width, rgb.data(), yuv.data());

    if (rtp.Transmit(yuv.data(), true) < 0) break;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (duration < 40) {
      std::this_thread::sleep_for(std::chrono::milliseconds(40 - duration));
    }

    frame++;
    std::cout << "Frame: " << frame << "\r" << std::flush;
  }
  std::cout << "\n";

  rtp.Close();

  std::cout << "Example terminated...\n";

  return 0;
}
