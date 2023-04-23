//
// Copyright (C) 2023 DefenceX PTY LTD. All rights reserved.
//
// This software is distributed under the included copyright license.
// Any redistribution or reproduction, in part or in whole, in any form or medium, is strictly prohibited without the
// prior written consent of DefenceX PTY LTD.
//
// For any inquiries or concerns, please contact:
// DefenceX PTY LTD
// Email: enquiries@defencex.ai
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

#include <csignal>
#include <iostream>
#include <vector>

#include "pngget.h"
#include "rtp_stream.h"

DEFINE_string(ipaddr, "239.192.1.1", "the IP address of the transmit stream");
DEFINE_int32(port, 5004, "the port to use for the transmit stream");
DEFINE_int32(height, 480, "the height of the image");
DEFINE_int32(width, 640, "the width of the image");
DEFINE_string(filename, "testcard.png", "the PNG file to use as the source of the video stream");

static bool running = true;

void signalHandler(int signum [[maybe_unused]]) { running = false; }

int main(int argc, char **argv) {
  uint32_t frame = 0;
  int move = 0;
  const int kBuffSize = (640 * 480) * 3;
  std::array<uint8_t, kBuffSize> yuv;
  std::array<uint8_t, kBuffSize> rtb_test;

  // register signal SIGINT and signal handler
  signal(SIGINT, signalHandler);

  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::cout << "Example RTP streaming to " << FLAGS_ipaddr.c_str() << ":" << FLAGS_port;

  // Setup RTP streaming class
  RtpStream rtp(FLAGS_height, FLAGS_width);
  rtp.RtpStreamOut(FLAGS_ipaddr, FLAGS_port);
  rtp.Open();

  memset(rtb_test.data(), 0, kBuffSize);
  std::vector<uint8_t> rgb = read_png_rgb24(FLAGS_filename);
  if (rgb.empty()) {
    std::cout << "Failed to read png file (" << FLAGS_filename << ")";
    return -1;
  }

  // Loop frames forever
  while (running) {
    // Convert all the scan lines
    RgbaToYuv(FLAGS_height, FLAGS_width, rgb.data(), yuv.data());

    if (rtp.Transmit(yuv.data()) < 0) break;

#if 1
    // move the image (png must have extra byte as the second image is green)
    move += 3;
    if (move == FLAGS_width * 3) move = 0;
#endif

    // approximately 24 frames a second
    // delay 40ms
    nanosleep((const struct timespec[]){{0, 1000000000L / kRtpFramerate}}, nullptr);

    /// delay 40ms

    std::cout << "Sent frame " << frame;
    frame++;
  }
  rtp.Close();

  std::cout << "Example terminated...\n";

  return 0;
}
