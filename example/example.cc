//
// MIT License
//
// Copyright (c) 2023 DefenceX (enquiries@defencex.ai)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the 'Software'), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
// THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
#include <string>
#include <vector>

#include "pngget.h"
#include "rtp_stream.h"

DEFINE_string(ipaddr, "239.192.1.1", "the IP address of the transmit stream");
DEFINE_int32(port, 5004, "the port to use for the transmit stream");
DEFINE_int32(height, 480, "the height of the image");
DEFINE_int32(width, 640, "the width of the image");
DEFINE_string(filename, "testcard.png", "the PNG file to use as the source of the video stream");

void DumpHex(uint8_t *data, size_t size) {
  std::string ascii = "                ";
  size_t i = 0;
  size_t j = 0;
  ascii[16] = '\0';
  for (i = 0; i < size; ++i) {
    printf("%02X ", (data)[i]);
    if ((data[i] >= ' ') && (data[i] <= '~')) {
      ascii[i % 16] = (data)[i];
    } else {
      ascii[i % 16] = '.';
    }
    if ((i + 1) % 8 == 0 || i + 1 == size) {
      printf(" ");
      if ((i + 1) % 16 == 0) {
        printf("|  %s \n", ascii.c_str());
      } else if (i + 1 == size) {
        ascii[(i + 1) % 16] = '\0';
        if ((i + 1) % 16 <= 8) {
          printf(" ");
        }
        for (j = (i + 1) % 16; j < 16; ++j) {
          printf("   ");
        }
        printf("|  %s \n", ascii.c_str());
      }
    }
  }
}

static bool running = true;

void signalHandler(int signum) { running = false; }

int main(int argc, char **argv) {
  uint32_t frame = 0;
  int move = 0;
  const int kBuffSize = (640 * 480) * 3;
  uint8_t yuv[kBuffSize];
  uint8_t rtb_test[kBuffSize];

  // register signal SIGINT and signal handler
  signal(SIGINT, signalHandler);

  gflags::ParseCommandLineFlags(&argc, &argv, true);

  printf("Example RTP streaming to %s:%d\n", FLAGS_ipaddr.c_str(), FLAGS_port);

  // Setup RTP streaming class
  RtpStream rtp(FLAGS_height, FLAGS_width);
  rtp.RtpStreamOut(FLAGS_ipaddr, FLAGS_port);
  rtp.Open();

  memset(rtb_test, 0, kBuffSize);
  std::vector<uint8_t> rgb = read_png_rgb24(FLAGS_filename);
  if (rgb.empty()) {
    printf("Failed to read png file (%s)\n", FLAGS_filename.c_str());
    return -1;
  }

  // Loop frames forever
  while (running) {
    // DumpHex((uint8_t *)rgb.data(), 64);

    // Convert all the scan lines
    RgbaToYuv(FLAGS_height, FLAGS_width, rgb.data(), (uint8_t *)yuv);

    DumpHex(yuv, 64);
    if (rtp.Transmit((uint8_t *)yuv) < 0) break;

#if 1
    // move the image (png must have extra byte as the second image is green)
    move += 3;
    if (move == FLAGS_width * 3) move = 0;
#endif

    // approximately 24 frames a second
    // delay 40ms
    nanosleep((const struct timespec[]){{0, 1000000000L / kRtpFramerate}}, nullptr);

    /// delay 40ms

    printf("Sent frame %d\n", frame++);
  }
  rtp.Close();

  printf("Example terminated...\n");

  return 0;
}
