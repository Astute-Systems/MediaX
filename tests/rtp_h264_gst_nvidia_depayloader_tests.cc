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
#include "rtp/rtp_utils.h"
#include "uncompressed/rtp_uncompressed_payloader.h"
#include "util_tests.h"
#include "utils/colourspace_cpu.h"

TEST(RtpH264VaapiDepayloaderTest, StateCheckNvidia) {
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
