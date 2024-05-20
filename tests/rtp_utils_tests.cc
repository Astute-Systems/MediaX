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

#include "rtp/rtp_types.h"
#include "rtp/rtp_utils.h"

TEST(Utilities, DumpHex) {
  std::string test = "This is a test string, it is 32 characters long";
  DumpHex(test.data(), test.size());
}

TEST(Utilities, EndianSwap16) {
  uint16_t test[4] = {0x1234, 0x5678, 0x9abc, 0xdef0};
  EndianSwap16(test, 4);
  ASSERT_EQ(test[0], 0x3412);
  ASSERT_EQ(test[1], 0x7856);
  ASSERT_EQ(test[2], 0xbc9a);
  ASSERT_EQ(test[3], 0xf0de);
}

TEST(Utilities, EndianSwap32) {
  uint32_t test[4] = {0x12345678, 0x9abcdef0, 0x12345678, 0x9abcdef0};
  EndianSwap32(test, 4);
  ASSERT_EQ(test[0], 0x78563412);
  ASSERT_EQ(test[1], 0xf0debc9a);
  ASSERT_EQ(test[2], 0x78563412);
  ASSERT_EQ(test[3], 0xf0debc9a);
}

TEST(Utilities, CreateColourBarTestCardRgb24) {
  const int kBuffSize = (640 * 480) * 3;
  std::array<uint8_t, kBuffSize> rtb_test = {0};
  CreateColourBarTestCard(rtb_test.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtb_test[0], 0xFF);
  ASSERT_EQ(rtb_test[1], 0x0);
  ASSERT_EQ(rtb_test[2], 0x0);
  ASSERT_EQ(rtb_test[3], 0xFF);
  ASSERT_EQ(rtb_test[4], 0x0);
  ASSERT_EQ(rtb_test[5], 0x0);
}

TEST(Utilities, CreateColourBarTestCardRgba) {
  const int kBuffSize = (640 * 480) * 4;
  std::array<uint8_t, kBuffSize> rtb_test = {0};
  CreateColourBarTestCard(rtb_test.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgba);
  ASSERT_EQ(rtb_test[0], 0xFF);
  ASSERT_EQ(rtb_test[1], 0x0);
  ASSERT_EQ(rtb_test[2], 0x0);
  ASSERT_EQ(rtb_test[3], 0x0);
  ASSERT_EQ(rtb_test[4], 0xFF);
  ASSERT_EQ(rtb_test[5], 0x0);
  ASSERT_EQ(rtb_test[6], 0x0);
  ASSERT_EQ(rtb_test[7], 0x0);
}

TEST(Utilities, CreateGreyScaleBarTestCardRgb) {
  const int kBuffSize = (640 * 480) * 3;
  std::array<uint8_t, kBuffSize> rtb_test = {0};
  CreateGreyScaleBarTestCard(rtb_test.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtb_test[0], 0);
  ASSERT_EQ(rtb_test[1], 0);
  ASSERT_EQ(rtb_test[2], 0);
}

TEST(Utilities, CreateComplexTestCardRgb) {
  const int kBuffSize = (640 * 480) * 3;
  std::array<uint8_t, kBuffSize> rtb_test = {0};
  CreateQuadTestCard(rtb_test.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtb_test[0], 0);
  ASSERT_EQ(rtb_test[1], 0);
  ASSERT_EQ(rtb_test[2], 0);
}

TEST(Utilities, CreateCheckeredTestCardRgb) {
  const int kBuffSize = (640 * 480) * 3;
  std::array<uint8_t, kBuffSize> rtb_test = {0};
  CreateCheckeredTestCard(rtb_test.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtb_test[0], 0xFF);
  ASSERT_EQ(rtb_test[1], 0xFF);
  ASSERT_EQ(rtb_test[2], 0xFF);
  ASSERT_EQ(rtb_test[4], 0xFF);
  ASSERT_EQ(rtb_test[5], 0xFF);
  ASSERT_EQ(rtb_test[6], 0xFF);
}

TEST(Utilities, CreateCheckeredTestCardRgba) {
  const int kBuffSize = (640 * 480) * 4;
  std::array<uint8_t, kBuffSize> rtb_test = {0};
  CreateCheckeredTestCard(rtb_test.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgba);
  ASSERT_EQ(rtb_test[0], 0xFF);
  ASSERT_EQ(rtb_test[1], 0xFF);
  ASSERT_EQ(rtb_test[2], 0xFF);
  ASSERT_EQ(rtb_test[3], 0x00);
  ASSERT_EQ(rtb_test[4], 0xFF);
  ASSERT_EQ(rtb_test[5], 0xFF);
  ASSERT_EQ(rtb_test[6], 0xFF);
  ASSERT_EQ(rtb_test[7], 0x00);
}

TEST(Utilities, BitsPerPixel) {
  EXPECT_EQ(mediax::BitsPerPixel(mediax::rtp::ColourspaceType::kColourspaceRgba), 32);
  EXPECT_EQ(mediax::BitsPerPixel(mediax::rtp::ColourspaceType::kColourspaceRgb24), 24);
  EXPECT_EQ(mediax::BitsPerPixel(mediax::rtp::ColourspaceType::kColourspaceYuv422), 16);
  EXPECT_EQ(mediax::BitsPerPixel(mediax::rtp::ColourspaceType::kColourspaceMono16), 16);
  EXPECT_EQ(mediax::BitsPerPixel(mediax::rtp::ColourspaceType::kColourspaceMono8), 8);
  EXPECT_EQ(mediax::BitsPerPixel(mediax::rtp::ColourspaceType::kColourspaceNv12), 12);
  EXPECT_EQ(mediax::BitsPerPixel(mediax::rtp::ColourspaceType::kColourspaceH264Part4), 24);
  EXPECT_EQ(mediax::BitsPerPixel(mediax::rtp::ColourspaceType::kColourspaceH264Part10), 24);
  EXPECT_EQ(mediax::BitsPerPixel(mediax::rtp::ColourspaceType::kColourspaceH265), 24);
}

TEST(Utilities, BytesPerPixel) {
  EXPECT_EQ(mediax::BytesPerPixel(mediax::rtp::ColourspaceType::kColourspaceRgba), 4);
  EXPECT_EQ(mediax::BytesPerPixel(mediax::rtp::ColourspaceType::kColourspaceRgb24), 3);
  EXPECT_EQ(mediax::BytesPerPixel(mediax::rtp::ColourspaceType::kColourspaceYuv422), 2);
  EXPECT_EQ(mediax::BytesPerPixel(mediax::rtp::ColourspaceType::kColourspaceMono16), 2);
  EXPECT_EQ(mediax::BytesPerPixel(mediax::rtp::ColourspaceType::kColourspaceMono8), 1);
  EXPECT_EQ(mediax::BytesPerPixel(mediax::rtp::ColourspaceType::kColourspaceNv12), 2);
  EXPECT_EQ(mediax::BytesPerPixel(mediax::rtp::ColourspaceType::kColourspaceH264Part4), 3);
  EXPECT_EQ(mediax::BytesPerPixel(mediax::rtp::ColourspaceType::kColourspaceH264Part10), 3);
  EXPECT_EQ(mediax::BytesPerPixel(mediax::rtp::ColourspaceType::kColourspaceH265), 3);
}