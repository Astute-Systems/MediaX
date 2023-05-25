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

namespace gva {

TEST(YuvToRgbTest, RedBufferTest) {
  uint32_t height = 10;
  uint32_t width = 10;
  uint8_t yuv[width * height * 2] = {0}; // Initialize to all zeros
  uint8_t rgb[width * height * 3] = {0}; // Initialize to all zeros

  // Fill the YUV buffer with a red color
  for (int i = 0; i < width * height * 2; i += 2) {
    yuv[i] = 0xFF; // Y component
    yuv[i + 1] = 0x00; // U component
  }

  // Fill the expected RGB buffer with a red color
  for (int i = 0; i < width * height * 3; i += 3) {
    rgb[i] = 0xFF; // R component
  }

  // Call the function being tested
  video::YuvToRgb(height, width, yuv, rgb);

  // Check that the RGB buffer matches the expected buffer
  for (int i = 0; i < width * height * 3; i++) {
    EXPECT_EQ(rgb[i], 0xFF);
  }
}

// Define a test fixture
class YuvToRgbaTest : public ::testing::Test {
 protected:
  // Define variables that can be used by all tests in the fixture
  uint32_t height_ = 10;
  uint32_t width_ = 10;
  uint8_t yuv_[8] = {0, 255, 0, 255, 0, 255, 0, 255};
  uint8_t rgba_[16] = {0};

  // Define a helper function that can be used by all tests in the fixture
  void CompareRgba(const uint8_t *expected, const uint8_t *actual, uint32_t size) {
    for (uint32_t i = 0; i < size; ++i) {
      EXPECT_EQ(expected[i], actual[i]) << "Mismatch at index " << i;
    }
  }
};

// Define a test case
TEST_F(YuvToRgbaTest, GreenInputBuffer) {
  // Call the function you want to test
  video::YuvToRgba(height_, width_, yuv_, rgba_);

  // Define the expected output
  uint8_t expected[16] = {0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255};

  // Compare the actual output with the expected output
  CompareRgba(expected, rgba_, 16);
}


}  // namespace gva
