//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \file testcard_tests.cc
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

TEST(TestCard, Solid) {
  mediax::video::ColourSpaceCpu colourspace;
  const uint32_t width = 640;
  const uint32_t height = 480;

  std::vector<uint8_t> test_card(width * height * 3);

  CreateSolidTestCard(test_card.data(), width, height, 0xFF, 0xFF, 0xFF,
                      mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_rgb24_white.png");
  CreateSolidTestCard(test_card.data(), width, height, 0xFF, 0xFF, 0xFF,
                      mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_rgb24_black.png");
  CreateSolidTestCard(test_card.data(), width, height, 0xFF, 0x00, 0x00,
                      mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_rgb24_red.png");
  CreateSolidTestCard(test_card.data(), width, height, 0x00, 0xFF, 0x00,
                      mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_rgb24_green.png");
  CreateSolidTestCard(test_card.data(), width, height, 0x00, 0x00, 0xFF,
                      mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_rgb24_blue.png");
}

TEST(TestCard, Checkered) {
  mediax::video::ColourSpaceCpu colourspace;
  const uint32_t width = 640;
  const uint32_t height = 480;

  std::vector<uint8_t> test_card(width * height * 3);

  CreateCheckeredTestCard(test_card.data(), width, height, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_rgb24_checkered.png");
}

TEST(TestCard, ColourBars) {
  mediax::video::ColourSpaceCpu colourspace;
  const uint32_t width = 640;
  const uint32_t height = 480;

  std::vector<uint8_t> test_card(width * height * 3);

  CreateColourBarTestCard(test_card.data(), width, height, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_rgb24_colourbars.png");
}

TEST(TestCard, GreyBars) {
  mediax::video::ColourSpaceCpu colourspace;
  const uint32_t width = 640;
  const uint32_t height = 480;

  std::vector<uint8_t> test_card(width * height * 3);

  CreateGreyScaleBarTestCard(test_card.data(), width, height, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_rgb24_greybars.png");
}

TEST(TestCard, CreateQuadTestCard) {
  mediax::video::ColourSpaceCpu colourspace;
  const uint32_t width = 640;
  const uint32_t height = 480;

  std::vector<uint8_t> test_card(width * height * 3);

  CreateQuadTestCard(test_card.data(), width, height, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_rgb24_quad.png");
}

TEST(TestCard, CreateBouncingBall) {
  mediax::video::ColourSpaceCpu colourspace;
  const uint32_t width = 640;
  const uint32_t height = 480;

  std::vector<uint8_t> test_card(width * height * 3);
  // Fill buffer zeros
  std::fill(test_card.begin(), test_card.end(), 0);
  std::vector<uint8_t> converted_test_card(width * height * 3);

  CreateBouncingBallTestCard(test_card.data(), width, height, mediax::rtp::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_rgb24_ball.png");
  // Fill buffer zeros
  std::fill(test_card.begin(), test_card.end(), 0);

  CreateBouncingBallTestCard(test_card.data(), width, height, mediax::rtp::ColourspaceType::kColourspaceMono8);
  colourspace.Mono8ToRgb(width, height, test_card.data(), converted_test_card.data());
  WritePngFile(converted_test_card.data(), width, height, "Testcard_mono8_ball.png");
  // Fill buffer zeros
  std::fill(test_card.begin(), test_card.end(), 0);

  CreateBouncingBallTestCard(test_card.data(), width, height, mediax::rtp::ColourspaceType::kColourspaceMono16);
  colourspace.Mono16ToRgb(width, height, test_card.data(), converted_test_card.data());
  WritePngFile(converted_test_card.data(), width, height, "Testcard_mono16_ball.png");
  // Fill buffer zeros
  std::fill(test_card.begin(), test_card.end(), 0);

  CreateBouncingBallTestCard(test_card.data(), width, height, mediax::rtp::ColourspaceType::kColourspaceYuv422);
  colourspace.YuvToRgb(width, height, test_card.data(), converted_test_card.data());
  WritePngFile(converted_test_card.data(), width, height, "Testcard_Yuv_ball.png");
}