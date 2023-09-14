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
  video::ColourSpaceCpu colourspace;
  const uint32_t width = 640;
  const uint32_t height = 480;

  std::vector<uint8_t> test_card(width * height * 3);

  CreateSolidTestCard(test_card.data(), width, height, 0xFF, 0xFF, 0xFF, mediax::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_white.png");
  CreateSolidTestCard(test_card.data(), width, height, 0xFF, 0xFF, 0xFF, mediax::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_black.png");
  CreateSolidTestCard(test_card.data(), width, height, 0xFF, 0x00, 0x00, mediax::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_red.png");
  CreateSolidTestCard(test_card.data(), width, height, 0x00, 0xFF, 0x00, mediax::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_green.png");
  CreateSolidTestCard(test_card.data(), width, height, 0x00, 0x00, 0xFF, mediax::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_blue.png");
}

TEST(TestCard, Checkered) {
  video::ColourSpaceCpu colourspace;
  const uint32_t width = 640;
  const uint32_t height = 480;

  std::vector<uint8_t> test_card(width * height * 3);

  CreateCheckeredTestCard(test_card.data(), width, height, mediax::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_checkered.png");
}

TEST(TestCard, ColourBars) {
  video::ColourSpaceCpu colourspace;
  const uint32_t width = 640;
  const uint32_t height = 480;

  std::vector<uint8_t> test_card(width * height * 3);

  CreateColourBarTestCard(test_card.data(), width, height, mediax::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_colourbars.png");
}

TEST(TestCard, GreyBars) {
  video::ColourSpaceCpu colourspace;
  const uint32_t width = 640;
  const uint32_t height = 480;

  std::vector<uint8_t> test_card(width * height * 3);

  CreateGreyScaleBarTestCard(test_card.data(), width, height, mediax::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_greybars.png");
}

TEST(TestCard, CreateQuadTestCard) {
  video::ColourSpaceCpu colourspace;
  const uint32_t width = 640;
  const uint32_t height = 480;

  std::vector<uint8_t> test_card(width * height * 3);

  CreateQuadTestCard(test_card.data(), width, height, mediax::ColourspaceType::kColourspaceRgb24);
  WritePngFile(test_card.data(), width, height, "Testcard_quad.png");
}
