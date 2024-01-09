//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \file create_test_cards_tests.cc
///

#include <glog/logging.h>
#include <gtest/gtest.h>

#include "rtp/rtp_utils.h"
#include "util_tests.h"

TEST(TestCards, Create) {
  // A 640x480 * 3 buffer vector
  std::vector<uint8_t> buffer(640 * 480 * 3);

  // Create a test pattern 0
  CreateColourBarEbuTestCard(buffer.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(buffer.data(), 640, 480, "CreateColourBarEbuTestCard.png");

  // Create a test pattern 0
  CreateColourBarTestCard(buffer.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(buffer.data(), 640, 480, "CreateColourBarTestCard.png");

  // Create a test card 1
  CreateGreyScaleBarTestCard(buffer.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(buffer.data(), 640, 480, "CreateGreyScaleBarTestCard.png");

  // Create a test card 2
  CreateQuadTestCard(buffer.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(buffer.data(), 640, 480, "CreateQuadTestCard.png");

  // Create a test card 3
  CreateCheckeredTestCard(buffer.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(buffer.data(), 640, 480, "CreateCheckeredTestCard.png");

  // Create a test card 4
  CreateSolidTestCard(buffer.data(), 640, 480, 0xff, 0x0, 0x0, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(buffer.data(), 640, 480, "CreateSolidTestCardRed.png");

  // Create a test card 5
  CreateSolidTestCard(buffer.data(), 640, 480, 0x00, 0xff, 0x0, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(buffer.data(), 640, 480, "CreateSolidTestCardGreen.png");

  // Create a test card 6
  CreateSolidTestCard(buffer.data(), 640, 480, 0x0, 0x0, 0xff, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(buffer.data(), 640, 480, "CreateSolidTestCardBlue.png");

  // Create a test card 7
  CreateSolidTestCard(buffer.data(), 640, 480, 0xff, 0x0, 0x0, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(buffer.data(), 640, 480, "CreateSolidTestCardRed.png");

  // Create a test card 8
  CreateSolidTestCard(buffer.data(), 640, 480, 0x0, 0x0, 0x0, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(buffer.data(), 640, 480, "CreateSolidTestCardBlack.png");

  // Create a test card 9
  CreateSolidTestCard(buffer.data(), 640, 480, 0xff, 0xff, 0xff, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(buffer.data(), 640, 480, "CreateSolidTestCardWhite.png");

  // Create a test card 10
  CreateWhiteNoiseTestCard(buffer.data(), 640, 480, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(buffer.data(), 640, 480, "CreateWhiteNoiseTestCard.png");
}