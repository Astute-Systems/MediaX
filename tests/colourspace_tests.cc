//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \file test_log.cc
///

#include <gtest/gtest.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "colourspace.h"
#include "rtp_utils.h"

namespace gva {

TEST(Colourspace, YuvToRgbTest) {
  uint32_t height = 4;
  uint32_t width = 4;
  uint8_t yuv[width * height * 2] = {0};          // Initialize to all zeros
  uint8_t rgb[width * height * 3] = {0};          // Initialize to all zeros
  uint8_t rgb_recieve[width * height * 3] = {0};  // Initialize to all zeros
  video::ColourSpace convert;

  // Fill the YUV buffer with a red color
  for (uint32_t i = 0; i < (width * height * 2) / 2; i += 4) {
    yuv[i] = 0x51;      // Y component
    yuv[i + 1] = 0x5a;  // U component
    yuv[i + 2] = 0x51;  // Y component
    yuv[i + 3] = 0xf0;  // U component
  }

  // Fill the expected RGB buffer with a red color
  for (uint32_t i = 0; i < width * height * 3; i += 3) {
    rgb[i] = 0xFC;  // R component
  }

  // Call the function being tested
  DumpHex(yuv, 16);
  convert.YuvToRgb(height, width, yuv, rgb_recieve);
  DumpHex(rgb_recieve, 16);

  // Check that the RGB buffer matches the expected buffer
  for (uint32_t i = 0; i < width * height; i++) {
    EXPECT_EQ(rgb[i], rgb_recieve[i]);
  }
}

// Define a test case
TEST(Colourspace, YuvToRgbaTest) {
  uint32_t height = 4;
  uint32_t width = 4;
  uint8_t yuv[width * height * 2] = {0};           // Initialize to all zeros
  uint8_t rgba[width * height * 4] = {0};          // Initialize to all zeros
  uint8_t rgba_recieve[width * height * 4] = {0};  // Initialize to all zeros
  video::ColourSpace convert;

  // Fill the YUV buffer with a red color
  for (uint32_t i = 0; i < (width * height * 2); i += 4) {
    yuv[i] = 0x51;      // Y component
    yuv[i + 1] = 0x5a;  // U component
    yuv[i + 2] = 0x51;  // Y component
    yuv[i + 3] = 0xf0;  // U component
  }
  DumpHex(yuv, 16);

  // Fill the expected RGB buffer with a red color
  for (uint32_t i = 0; i < width * height * 4; i += 4) {
    rgba[i] = 0xFC;  // R component
  }
  // Call the function you want to test
  convert.YuvToRgba(height, width, yuv, rgba_recieve);
  DumpHex(rgba_recieve, 16);

  // Check that the RGB buffer matches the expected buffer
  // for (int i = 0; i < width * height; i++) {
  //   EXPECT_EQ(rgba[i], rgba_recieve[i]);
  // }
}

TEST(Colourspace, RgbToYuvTest) {
  // Create a 4x4 RGB buffer
  const uint32_t width = 4;
  const uint32_t height = 4;
  const uint32_t bufferSize = width * height * 3;
  uint8_t rgb[bufferSize] = {0};
  video::ColourSpace convert;

  // Fill the expected RGB buffer with a red color
  for (uint32_t i = 0; i < width * height * 3; i += 3) {
    rgb[i] = 0xFF;  // R component
  }

  // Call the function you want to test
  uint8_t yuv[bufferSize * 2] = {0};
  convert.RgbToYuv(height, width, rgb, yuv);

  DumpHex(yuv, 16);
}

TEST(Colourspace, RgbaToYuvTest) {
  // Create a 4x4 RGB buffer
  const uint32_t width = 4;
  const uint32_t height = 4;
  const uint32_t bufferSize = width * height * 4;
  uint8_t rgba[bufferSize] = {0};
  video::ColourSpace convert;

  // Fill the expected RGB buffer with a red color
  for (uint32_t i = 0; i < width * height * 4; i += 4) {
    rgba[i] = 0xFF;  // R component
  }

  // Call the function you want to test
  uint8_t yuv[bufferSize * 2] = {0};
  convert.RgbaToYuv(height, width, rgba, yuv);

  DumpHex(yuv, 16);
}

TEST(Colourspace, RgbaToRgbTest) {
  // Create a 4x4 RGB buffer
  const uint32_t width = 4;
  const uint32_t height = 4;
  const uint32_t bufferSize = width * height * 4;
  uint8_t rgba[bufferSize] = {0};
  video::ColourSpace convert;

  // Fill the expected RGB buffer with a red color
  for (uint32_t i = 0; i < width * height * 4; i += 4) {
    rgba[i] = 0xFF;  // R component
  }

  // Call the function you want to test
  uint8_t rgb[bufferSize * 2] = {0};
  convert.RgbaToRgb(height, width, rgba, rgb);

  DumpHex(rgb, 16);
}

}  // namespace gva
