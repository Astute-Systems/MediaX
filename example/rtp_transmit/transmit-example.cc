//
// Copyright (c) 2024, DefenceX PTY LTD
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
/// \brief A simple video transmitter example
///
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
#include "sap/sap_announcer.h"
#include "v4l2/v4l2_source.h"
#include "version.h"

DEFINE_string(ipaddr, kIpAddressDefault, "the IP address of the transmit stream");
DEFINE_uint32(port, kPortDefault, "the port to use for the transmit stream");
DEFINE_uint32(height, kHeightDefault, "the height of the image");
DEFINE_uint32(width, kWidthDefault, "the width of the image");
DEFINE_uint32(framerate, 25, "the image framerate");
DEFINE_uint32(source, 2,
              "The video source (0-10)\n\t"
              "0 - Use a PNG file (see -filename)\n\t"
              "1 - v4l2src\n\t"
              "2 - EBU Colour bars \n\t"
              "3 - Greyscale bars\n\t"
              "4 - Scaled RGB values\n\t"
              "5 - Chequered test card\n\t"
              "6 - Solid white\n\t"
              "7 - Solid black\n\t"
              "8 - Solid red\n\t"
              "9 - Solid green\n\t"
              "10 - Solid blue\n\t"
              "11 - White noise\n\t"
              "12 - Colour bars \n\t");

DEFINE_string(filename, "testcard.png", "the PNG file to use as the source of the video stream (only with -source 0)");
DEFINE_string(device, "/dev/video0", "the V4L2 device source (only with -source 1)");
DEFINE_string(session_name, "TestVideo1", "the SAP/SDP session name");
DEFINE_uint32(mode, 1,
              "The video mode (0-4)\n\t"
              "0 - Uncompressed RGB\n\t"
              "1 - Uncompressed YUV\n\t"
              "2 - Mono16\n\t"
#if GST_SUPPORTED
              "3 - Mono8\n\t"
              "4 - H.264\n\t"
              "5 - H.265\n\t");
#else
              "3 - Mono8\n\t");
#endif
DEFINE_uint32(num_frames, 0, "The number of frames to send");
DEFINE_bool(verbose, false, "For verbose output");

static bool application_running = true;

///
/// \brief The signal handler
///
/// \param signum the signal number
///
void signalHandler(int signum [[maybe_unused]]) {
  std::cout << "transmit-example Interrupt signal (" << signum << ") received.\n";
  application_running = false;
  // Sleep 100ms
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

///
/// \brief The main entry point
///
/// \param argc the argument count
/// \param argv the argument values
/// \return int the return code
///
int main(int argc, char** argv) {
#if CUDA_ENABLED
  std::shared_ptr<mediax::video::ColourSpace> convert = std::make_shared<mediax::video::ColourSpaceCuda>();
#else
  std::shared_ptr<mediax::video::ColourSpace> convert = std::make_shared<mediax::video::ColourSpaceCpu>();
#endif
  mediax::rtp::ColourspaceType video_mode = mediax::rtp::ColourspaceType::kColourspaceYuv;

  std::unique_ptr<V4L2Capture> v4lsource;

  gflags::SetVersionString(mediax::kVersion);
  gflags::SetUsageMessage(
      "Transmit a video stream using RTP\n"
      "Usage:\n"
      "    rtp-transmit [options]\n"
      "Example:\n"
      "    rtp-transmit -ipaddr 127.0.0.1 -port 5004 -height 480 -width 640 -source 0\n");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  mediax::InitRtp(argc, argv);

  uint32_t frame = 1;
  const uint32_t kBuffSize = FLAGS_height * FLAGS_width;
  const uint32_t kBuffSizeRGB = kBuffSize * 3;
  std::vector<uint8_t> transmit_buffer;  // Maximum is HD

  // register signal SIGINT and signal handler
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  std::cout << "Example RTP (Tx) streaming (" << FLAGS_width << "x" << FLAGS_height << " " << ModeToString(FLAGS_mode)
            << ") to " << FLAGS_ipaddr.c_str() << ":" << FLAGS_port << "@" << FLAGS_framerate << "Htz\n";

  video_mode = GetMode(FLAGS_mode);
  transmit_buffer.resize(FLAGS_height * FLAGS_width * ::mediax::BytesPerPixel(video_mode));

  // Setup RTP streaming class
  std::unique_ptr<mediax::rtp::RtpPayloader> rtp;
#if GST_SUPPORTED
  switch (video_mode) {
    default:  // Assume uncompressed
      rtp = std::make_unique<mediax::rtp::uncompressed::RtpUncompressedPayloader>();
      break;
    case mediax::rtp::ColourspaceType::kColourspaceH264Part10:
    case mediax::rtp::ColourspaceType::kColourspaceH264Part4:
      rtp = std::make_unique<mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiPayloader>();
      break;
    case mediax::rtp::ColourspaceType::kColourspaceH265:
      rtp = std::make_unique<mediax::rtp::h265::gst::vaapi::RtpH265GstVaapiPayloader>();
      break;
  }
#else
  rtp = std::make_unique<mediax::RtpUncompressedPayloader>();
#endif

  // Get the SAP/SDP announcment singleton instance
  mediax::sap::SapAnnouncer& sap_announcer = mediax::sap::SapAnnouncer::GetInstance();

  // Create a stream information object
  mediax::rtp::StreamInformation stream_information = {FLAGS_filename, FLAGS_ipaddr, (uint16_t)FLAGS_port,
                                                       FLAGS_height,   FLAGS_width,  FLAGS_framerate,
                                                       video_mode,     false};
  // Add a SAP announcement for the new stream
  sap_announcer.AddSapAnnouncement(stream_information);
  sap_announcer.Start();
  // Add a SAP announcement for the new stream
  rtp->SetStreamInfo(stream_information);
  rtp->Open();

  // Read the PNG file
  std::vector<uint8_t> video_buffer;
  switch (FLAGS_source) {
    case 1:
      video_buffer.resize(kBuffSizeRGB);
      v4lsource = std::make_unique<V4L2Capture>(FLAGS_device, FLAGS_height, FLAGS_width);
      if (v4lsource->Initalise() != 0) {
        LOG(ERROR) << "Failed to initalise V4l2 device";
        return 1;
      }
      v4lsource->CaptureFrame(video_buffer.data());
      LOG(INFO) << "Creating V4l2 source device=" + FLAGS_device;
      break;
    case 2:  // EBU Colour bars
      video_buffer.resize(kBuffSizeRGB);
      CreateColourBarEbuTestCard(video_buffer.data(), FLAGS_width, FLAGS_height, video_mode);
      LOG(INFO) << "Creating colour bar test card";
      break;
    case 3:
      video_buffer.resize(kBuffSizeRGB);
      CreateGreyScaleBarTestCard(video_buffer.data(), FLAGS_width, FLAGS_height, video_mode);
      LOG(INFO) << "Creating greyscale test card";
      break;
    case 4:
      video_buffer.resize(kBuffSizeRGB);
      CreateQuadTestCard(video_buffer.data(), FLAGS_width, FLAGS_height, video_mode);
      LOG(INFO) << "Creating scaled RGB values";
      break;
    case 5:
      video_buffer.resize(kBuffSizeRGB);
      CreateCheckeredTestCard(video_buffer.data(), FLAGS_width, FLAGS_height, video_mode);
      LOG(INFO) << "Creating checkered test card";
      break;
    case 6:  // Solid white
      video_buffer.resize(kBuffSizeRGB);
      CreateSolidTestCard(video_buffer.data(), FLAGS_width, FLAGS_height, 255, 255, 255, video_mode);
      LOG(INFO) << "Creating solid white test card";
      break;
    case 7:  // Solid black
      video_buffer.resize(kBuffSizeRGB);
      CreateSolidTestCard(video_buffer.data(), FLAGS_width, FLAGS_height, 0, 0, 0, video_mode);
      LOG(INFO) << "Creating solid black test card";
      break;
    case 8:  // Solid red
      video_buffer.resize(kBuffSizeRGB);
      CreateSolidTestCard(video_buffer.data(), FLAGS_width, FLAGS_height, 255, 0, 0, video_mode);
      LOG(INFO) << "Creating solid red test card";
      break;
    case 9:  // Solid green
      video_buffer.resize(kBuffSizeRGB);
      CreateSolidTestCard(video_buffer.data(), FLAGS_width, FLAGS_height, 0, 255, 0, video_mode);
      LOG(INFO) << "Creating solid green test card";
      break;
    case 10:  // Solid blue
      video_buffer.resize(kBuffSizeRGB);
      CreateSolidTestCard(video_buffer.data(), FLAGS_width, FLAGS_height, 0, 0, 255, video_mode);
      LOG(INFO) << "Creating solid blue test card";
      break;
    case 11:  // White noise
      video_buffer.resize(kBuffSizeRGB);
      CreateWhiteNoiseTestCard(video_buffer.data(), FLAGS_width, FLAGS_height, video_mode);
      LOG(INFO) << "Creating white noise test card";
      break;
    case 12:  // Colourbars
      video_buffer.resize(kBuffSizeRGB);
      CreateColourBarTestCard(video_buffer.data(), FLAGS_width, FLAGS_height, video_mode);
      LOG(INFO) << "Creating colour bar test card";
      break;
    case 0:
      Png image_reader;
      video_buffer = image_reader.ReadPngRgb24(FLAGS_filename);
      if (video_buffer.empty()) {
        LOG(INFO) << "Failed to read png file (" << FLAGS_filename << ")";
        return -1;
      }
      /// Make it RGB
      convert->RgbaToRgb(FLAGS_height, FLAGS_width, video_buffer.data(), video_buffer.data());
      break;
    default:
      LOG(INFO) << "Invalid source mode (" << FLAGS_source << ")";
      break;
  }

  sap_announcer.Start();
  rtp->Start();

  // Convert all the scan lines
  // Loop frames forever
  uint32_t count = 1;
  int32_t interval = 1000 / FLAGS_framerate;
  auto start = std::chrono::high_resolution_clock::now();

  while (application_running == true) {
    // Clear the YUV buffer
    if (FLAGS_source == 1) {
      v4lsource->CaptureFrame(video_buffer.data());
    } else {
      if (FLAGS_source == 11) {
        // Creat a new white noise test card
        CreateWhiteNoiseTestCard(transmit_buffer.data(), FLAGS_width, FLAGS_height, video_mode);
      } else {
        // Clear buffer
        memset(transmit_buffer.data(), 0, FLAGS_height * FLAGS_width * ::mediax::BytesPerPixel(video_mode));
        // Copy new image into buffer
        memcpy(transmit_buffer.data(), video_buffer.data(),
               FLAGS_height * FLAGS_width * ::mediax::BytesPerPixel(video_mode));
      }
    }

    auto end = std::chrono::high_resolution_clock::now();

    if (auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        duration < interval) {
      std::this_thread::sleep_for(std::chrono::milliseconds(interval - duration));
    } else {
      std::cout << "Frame took too long to process, overrun by " << duration - interval << "ms\n";
    }
    // Timestamp start of transmission
    start = std::chrono::high_resolution_clock::now();

    // Set buffer to 0xff
    if (rtp->Transmit(transmit_buffer.data(), true) < 0) {
      std::cerr << "Failed to transmit frame\n";
    }

    if (FLAGS_num_frames > 0) {
      // Bail out if we are counting frames and meet the requested limit
      if (count > FLAGS_num_frames) {
        break;
      }
    }

    if (FLAGS_verbose) std::cout << "Frame=" << count << "\r" << std::flush;
    count++;
  }

  if (FLAGS_verbose) std::cout << "\n";

  sap_announcer.Stop();
  rtp->Stop();
  rtp->Close();
  mediax::RtpCleanup();

  std::cout << "RTP (Tx) Example terminated...\n";

  return 0;
}
