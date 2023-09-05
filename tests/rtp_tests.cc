//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \file rtp_utils_tests.cc
///

#include <gtest/gtest.h>
#include <unistd.h>

#include "rtp/rtp.h"
#include "rtp/rtp_utils.h"

TEST(Rtp, Init) { mediax::RtpInit(0, nullptr); }

TEST(Rtp, CleanUp) { mediax::RtpCleanup(); }

TEST(Rtp, Bits) {
  EXPECT_EQ(mediax::BitsPerPixel(mediax::ColourspaceType::kColourspaceRgba), 32);
  EXPECT_EQ(mediax::BitsPerPixel(mediax::ColourspaceType::kColourspaceRgb24), 24);
  EXPECT_EQ(mediax::BitsPerPixel(mediax::ColourspaceType::kColourspaceYuv), 16);
  EXPECT_EQ(mediax::BitsPerPixel(mediax::ColourspaceType::kColourspaceMono16), 16);
  EXPECT_EQ(mediax::BitsPerPixel(mediax::ColourspaceType::kColourspaceMono8), 8);
}
