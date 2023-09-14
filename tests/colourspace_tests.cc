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

#include "rtp/rtp_utils.h"
#include "util_tests.h"
#include "utils/colourspace_cpu.h"

TEST(Colourspace, YuvToRgbTest) {
  uint32_t height = 4;
  uint32_t width = 4;
  uint8_t yuv[width * height * 2];
  uint8_t rgb[width * height * 3];
  uint8_t rgb_receive[width * height * 3];
  video::ColourSpaceCpu convert;

  // Fill the YUV buffer with a red color
  for (uint32_t i = 0; i < (width * height * 2) / 2; i += 4) {
    yuv[i] = 0x51;      // Y component
    yuv[i + 1] = 0x5a;  // U component
    yuv[i + 2] = 0x51;  // Y component
    yuv[i + 3] = 0xf0;  // U component
  }

  // Fill the expected RGB buffer with a red color
  for (uint32_t i = 0; i < width * height * 3; i += 3) {
    rgb[i] = 0xFC;      // R component
    rgb[i + 1] = 0x00;  // G component
    rgb[i + 2] = 0x00;  // B component
  }

  // Call the function being tested
  DumpHex(yuv, 16);
  convert.YuvToRgb(height, width, yuv, rgb_receive);
  DumpHex(rgb_receive, 16);
  DumpHex(rgb, 16);

  // Check that the RGB buffer matches the expected buffer
  for (uint32_t i = 0; i < width * height; i++) {
    EXPECT_EQ(rgb_receive[i], rgb_receive[i]);
  }
}

TEST(Colourspace, YuvToRgbaTest) {
  uint32_t height = 4;
  uint32_t width = 4;
  uint8_t yuv[width * height * 2];
  uint8_t rgba_recieve[width * height * 4];
  video::ColourSpaceCpu convert;

  // Fill the YUV buffer with a red color
  for (uint32_t i = 0; i < (width * height * 2); i += 4) {
    yuv[i] = 0x51;      // Y component
    yuv[i + 1] = 0x5a;  // U component
    yuv[i + 2] = 0x51;  // Y component
    yuv[i + 3] = 0xf0;  // U component
  }
  DumpHex(yuv, 16);

  // Call the function you want to test
  convert.YuvToRgba(height, width, yuv, rgba_recieve);
  DumpHex(rgba_recieve, 16);
}

TEST(Colourspace, RgbToRgbaTest) {
  uint32_t height = 4;
  uint32_t width = 4;
  uint8_t rgb[width * height * 3];
  uint8_t rgba_recieve[width * height * 4];
  video::ColourSpaceCpu convert;

  // Fill the RGB buffer with a red color
  for (uint32_t i = 0; i < (width * height * 3); i += 3) {
    rgb[i] = 0xFF;      // R component
    rgb[i + 1] = 0x00;  // G component
    rgb[i + 2] = 0x00;  // B component
  }
  DumpHex(rgb, 16);

  // Call the function you want to test
  convert.RgbToRgba(height, width, rgb, rgba_recieve);
  DumpHex(rgba_recieve, 16);
}

TEST(Colourspace, Mono8ToRgbaTest) {
  uint32_t height = 4;
  uint32_t width = 4;
  uint8_t mono8[width * height];
  uint8_t rgba_recieve[width * height * 4];
  video::ColourSpaceCpu convert;

  // Fill the RGB buffer with a red color
  for (uint32_t i = 0; i < (width * height); i++) {
    mono8[i] = 0xFF;  // R component
  }
  DumpHex(mono8, 16);

  // Call the function you want to test
  convert.Mono8ToRgba(height, width, mono8, rgba_recieve);
  DumpHex(rgba_recieve, 16);
}

TEST(Colourspace, Mono16ToRgbaTest) {
  uint32_t height = 4;
  uint32_t width = 4;
  uint8_t mono16[width * height * 2];
  uint8_t rgba_recieve[width * height * 4];
  video::ColourSpaceCpu convert;

  // Fill the RGB buffer with a red color
  for (uint32_t i = 0; i < (width * height * 2); i += 2) {
    mono16[i] = 0xFF;      // R component
    mono16[i + 1] = 0x00;  // R component
  }
  DumpHex(mono16, 16);

  // Call the function you want to test
  convert.Mono16ToRgba(height, width, mono16, rgba_recieve);
  DumpHex(rgba_recieve, 16);
}

TEST(Colourspace, RgbToYuvTest) {
  // Create a 4x4 RGB buffer
  const uint32_t width = 4;
  const uint32_t height = 4;
  const uint32_t bufferSize = width * height * 3;
  uint8_t rgb[bufferSize];
  video::ColourSpaceCpu convert;

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
  video::ColourSpaceCpu convert;

  // Fill the expected RGB buffer with a red color
  for (uint32_t i = 0; i < width * height * 4; i += 4) {
    rgba[i] = 0xFF;  // R component
  }

  // Call the function you want to test
  uint8_t yuv[bufferSize * 2] = {0};
  convert.RgbaToYuv(height, width, rgba, yuv);

  DumpHex(yuv, 16);
}

TEST(Colourspace, RgbToMono8Test) {
  // Create a 4x4 RGB buffer
  const uint32_t width = 4;
  const uint32_t height = 4;
  const uint32_t bufferSize = width * height * 3;
  uint8_t rgb[bufferSize];
  video::ColourSpaceCpu convert;

  // Fill the expected RGB buffer with a red color
  for (uint32_t i = 0; i < width * height * 3; i += 3) {
    rgb[i] = 0xFF;  // R component
  }

  // Call the function you want to test
  uint8_t mono[bufferSize * 2] = {0};
  convert.RgbToMono8(height, width, rgb, mono);

  DumpHex(mono, 16);
}

TEST(Colourspace, RgbToMono16Test) {
  // Create a 4x4 RGB buffer
  const uint32_t width = 4;
  const uint32_t height = 4;
  const uint32_t bufferSize = width * height * 3;
  uint8_t rgb[bufferSize];
  video::ColourSpaceCpu convert;

  // Fill the expected RGB buffer with a red color
  for (uint32_t i = 0; i < width * height * 3; i += 3) {
    rgb[i] = 0xFF;  // R component
  }

  // Call the function you want to test
  uint8_t mono[bufferSize * 2] = {0};
  convert.RgbToMono16(height, width, rgb, mono);

  DumpHex(mono, 16);
}

TEST(Colourspace, RgbaToRgbTest) {
  // Create a 4x4 RGB buffer
  const uint32_t width = 4;
  const uint32_t height = 4;
  const uint32_t bufferSize = width * height * 4;
  uint8_t rgba[bufferSize] = {0};
  video::ColourSpaceCpu convert;

  // Fill the expected RGB buffer with a red color
  for (uint32_t i = 0; i < width * height * 4; i += 4) {
    rgba[i] = 0xFF;  // R component
  }

  // Call the function you want to test
  uint8_t rgb[bufferSize * 2] = {0};
  convert.RgbaToRgb(height, width, rgba, rgb);

  DumpHex(rgb, 16);
}

TEST(Colourspace, ScaleToSizeTest1) {
  // Define input parameters
  const uint32_t source_height = 1080;
  const uint32_t source_width = 1920;
  uint8_t source_rgb_buffer[source_height * source_width * 3] = {0};  // Initialize with zeros
  const uint32_t target_height = 480;
  const uint32_t target_width = 640;
  uint8_t target_rgb_buffer[target_height * target_width * 3] = {0};  // Initialize with zeros

  // Source is checked
  CreateCheckeredTestCard(source_rgb_buffer, source_width, source_height, mediax::ColourspaceType::kColourspaceRgb24);
  // Colour bars in target
  CreateColourBarTestCard(target_rgb_buffer, target_width, target_height, mediax::ColourspaceType::kColourspaceRgb24);

  // Call the function
  video::ColourSpaceCpu colourspace;
  colourspace.ScaleToSize(source_height, source_width, source_rgb_buffer, target_height, target_width,
                          target_rgb_buffer);

  // Write the result to a file
  WritePngFile(target_rgb_buffer, target_width, target_height, "1920x1080_scaled_to_640x480.png");
}

TEST(Colourspace, ScaleToSizeTest2) {
  // Define input parameters
  const uint32_t source_height = 480;
  const uint32_t source_width = 640;
  uint8_t source_rgb_buffer[source_height * source_width * 3] = {0};  // Initialize with zeros
  const uint32_t target_height = 1080;
  const uint32_t target_width = 1920;
  uint8_t target_rgb_buffer[target_height * target_width * 3] = {0};  // Initialize with zeros

  // Source is checked
  CreateCheckeredTestCard(source_rgb_buffer, source_width, source_height, mediax::ColourspaceType::kColourspaceRgb24);
  // Colour bars in target
  CreateColourBarTestCard(target_rgb_buffer, target_width, target_height, mediax::ColourspaceType::kColourspaceRgb24);

  // Call the function
  video::ColourSpaceCpu colourspace;
  colourspace.ScaleToSize(source_height, source_width, source_rgb_buffer, target_height, target_width,
                          target_rgb_buffer);

  // Write the result to a file
  WritePngFile(target_rgb_buffer, target_width, target_height, "640x480_scaled_to_1920x1080.png");
}
