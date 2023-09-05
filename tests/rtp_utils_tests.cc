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

TEST(Utilities, CreateColourBarTestCard) {
  const int kBuffSize = (640 * 480) * 3;
  std::array<uint8_t, kBuffSize> rtb_test = {0};
  CreateColourBarTestCard(rtb_test.data(), 640, 480, mediax::ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtb_test[0], 0xFF);
  ASSERT_EQ(rtb_test[1], 0);
  ASSERT_EQ(rtb_test[2], 0x0);
}

TEST(Utilities, CreateGreyScaleBarTestCard) {
  const int kBuffSize = (640 * 480) * 3;
  std::array<uint8_t, kBuffSize> rtb_test = {0};
  CreateGreyScaleBarTestCard(rtb_test.data(), 640, 480, mediax::ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtb_test[0], 0);
  ASSERT_EQ(rtb_test[1], 0);
  ASSERT_EQ(rtb_test[2], 0);
}

TEST(Utilities, CreateComplexTestCard) {
  const int kBuffSize = (640 * 480) * 3;
  std::array<uint8_t, kBuffSize> rtb_test = {0};
  CreateComplexTestCard(rtb_test.data(), 640, 480, mediax::ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtb_test[0], 0);
  ASSERT_EQ(rtb_test[1], 0);
  ASSERT_EQ(rtb_test[2], 0);
}

TEST(Utilities, CreateCheckeredTestCard) {
  const int kBuffSize = (640 * 480) * 3;
  std::array<uint8_t, kBuffSize> rtb_test = {0};
  CreateCheckeredTestCard(rtb_test.data(), 640, 480, mediax::ColourspaceType::kColourspaceRgb24);
  ASSERT_EQ(rtb_test[0], 0xFF);
  ASSERT_EQ(rtb_test[1], 0xFF);
  ASSERT_EQ(rtb_test[2], 0xFF);
}
