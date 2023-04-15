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

DEFINE_string(ipaddr, "127.0.0.1", "the IP address of the transmit stream");
DEFINE_int32(port, 5004, "the port to use for the transmit stream");
DEFINE_int32(height, 480, "the height of the image");
DEFINE_int32(width, 640, "the width of the image");
DEFINE_string(filename, "testcard.png", "the PNG file to use as the source of the video stream");

void DumpHex(const int8_t *data, size_t size) {
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

void RGB24toYUV422(int height, int width, char *rgb_buffer, char *yuv_buffer) {
  int8_t r = 0;
  int8_t g = 0;
  int8_t b = 0;
  int32_t y1 = 0;
  int32_t y2 = 0;
  int32_t u = 0;
  int32_t v = 0;
  int32_t size = width * height;

  // Convert each 2x1 pixel block from RGB to YUV
  for (int i = 0; i < size; i += 2) {
    r = *rgb_buffer++;
    g = *rgb_buffer++;
    b = *rgb_buffer++;
    y1 = (int)(0.299 * r + 0.587 * g + 0.114 * b);
    u = (int)((b - y1) * 0.564 + 128);
    v = (int)((r - y1) * 0.713 + 128);

    r = *rgb_buffer++;
    g = *rgb_buffer++;
    b = *rgb_buffer++;
    y2 = (int)(0.299 * r + 0.587 * g + 0.114 * b);

    // Interpolate the U and V values for the second pixel
    u = (u + (int)((b - y2) * 0.564 + 128)) / 2;
    v = (v + (int)((r - y2) * 0.713 + 128)) / 2;

    // Clip the YUV values to the range [0, 255]
    y1 = (y1 < 0) ? 0 : ((y1 > 255) ? 255 : y1);
    u = (u < 0) ? 0 : ((u > 255) ? 255 : u);
    v = (v < 0) ? 0 : ((v > 255) ? 255 : v);

    y2 = (y2 < 0) ? 0 : ((y2 > 255) ? 255 : y2);

    // Write the YUV values to the output buffer
    *yuv_buffer++ = (unsigned char)y1;
    *yuv_buffer++ = (unsigned char)u;
    *yuv_buffer++ = (unsigned char)y2;
    *yuv_buffer++ = (unsigned char)v;
  }
}

static bool running = true;

void signalHandler(int signum) { running = false; }

int main(int argc, char **argv) {
  uint32_t frame = 0;
  int move = 0;
  const int kBuffSize = (640 * 480) * 3;
  int8_t yuv[kBuffSize];
  int8_t rtb_test[kBuffSize];

  // register signal SIGINT and signal handler
  signal(SIGINT, signalHandler);

  gflags::ParseCommandLineFlags(&argc, &argv, true);

  printf("Example RTP streaming\n");

  // Setup RTP streaming class
  RtpStream rtp(FLAGS_height, FLAGS_width);
  rtp.RtpStreamOut(FLAGS_ipaddr, FLAGS_port);
  rtp.Open();

  memset(rtb_test, 0, kBuffSize);
  std::vector<uint8_t> rgb = read_png_rgb24(FLAGS_filename);
  if (rgb.size() == 0) {
    printf("Failed to read png file\n");
    return -1;
  }

  // Loop frames forever
  while (running) {
    // Convert all the scan lines
    RgbToYuv(FLAGS_height, FLAGS_width, rgb.data(), (uint8_t *)yuv);

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
