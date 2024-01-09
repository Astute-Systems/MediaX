#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "utils/colourspace_cpu.h"

TEST(ColourSpaceCpuTest, ScaleToSizeRgb_NullPointers) {
  mediax::video::ColourSpaceCpu colourSpaceCpu;

  uint8_t* source_rgb_buffer = nullptr;
  uint8_t* target_rgb_buffer = nullptr;
  uint32_t source_height = 480;
  uint32_t source_width = 640;
  uint32_t target_height = 240;
  uint32_t target_width = 320;

  int result = colourSpaceCpu.ScaleToSizeRgb(source_height, source_width, source_rgb_buffer, target_height,
                                             target_width, target_rgb_buffer);

  EXPECT_EQ(result, 1);
}

TEST(ColourSpaceCpuTest, ScaleToSizeRgb_AllocationFailure) {
  mediax::video::ColourSpaceCpu colourSpaceCpu;

  uint8_t source_rgb_buffer[640 * 480 * 3];
  uint8_t* target_rgb_buffer = nullptr;
  uint32_t source_height = 480;
  uint32_t source_width = 640;
  uint32_t target_height = 240;
  uint32_t target_width = 320;

  int result = colourSpaceCpu.ScaleToSizeRgb(source_height, source_width, source_rgb_buffer, target_height,
                                             target_width, target_rgb_buffer);

  EXPECT_EQ(result, 1);
}

TEST(ColourSpaceCpuTest, ScaleToSizeRgb_Success) {
  mediax::video::ColourSpaceCpu colourSpaceCpu;

  uint8_t source_rgb_buffer[640 * 480 * 3];
  uint8_t target_rgb_buffer[320 * 240 * 3];
  uint32_t source_height = 480;
  uint32_t source_width = 640;
  uint32_t target_height = 240;
  uint32_t target_width = 320;

  int result = colourSpaceCpu.ScaleToSizeRgb(source_height, source_width, source_rgb_buffer, target_height,
                                             target_width, target_rgb_buffer);

  EXPECT_EQ(result, 0);
}

TEST(ColourSpaceCpuTest, ScaleToSizeRgba_NullPointers) {
  mediax::video::ColourSpaceCpu colourSpaceCpu;
  uint8_t source_rgb_buffer[640 * 480 * 3]; 
  uint8_t target_rgba_buffer[640 * 480 * 4]; 
  uint32_t source_height = 480;
  uint32_t source_width = 640;
  uint32_t target_height = 720;
  uint32_t target_width = 1280;

  int result = colourSpaceCpu.ScaleToSizeRgba(source_height, source_width, source_rgb_buffer, target_height,
                                              target_width, target_rgba_buffer);

  EXPECT_EQ(result, 1);
}

TEST(ColourSpaceCpuTest, ScaleToSizeRgba_AllocationFailure) {
  mediax::video::ColourSpaceCpu colourSpaceCpu;
  std::vector<uint8_t> source_rgb_buffer(640 * 480 * 4);
  std::vector<uint8_t> target_rgba_buffer(1280 * 720 * 4);
  uint32_t source_height = 480;
  uint32_t source_width = 640;
  uint32_t target_height = 720;
  uint32_t target_width = 1280;

  // Simulate allocation failure by passing invalid target_width and target_height
  int result = colourSpaceCpu.ScaleToSizeRgba(source_height, source_width, source_rgb_buffer.data(), 0, 0,
                                              target_rgba_buffer.data());

  EXPECT_EQ(result, 2);
}

TEST(ColourSpaceCpuTest, ScaleToSizeRgba_Success) {
  mediax::video::ColourSpaceCpu colourSpaceCpu;
  std::vector<uint8_t> source_rgb_buffer(640 * 480 * 4);
  std::vector<uint8_t> target_rgba_buffer(1280 * 720 * 4);
  uint32_t source_height = 480;
  uint32_t source_width = 640;
  uint32_t target_height = 720;
  uint32_t target_width = 1280;

  int result = colourSpaceCpu.ScaleToSizeRgba(source_height, source_width, source_rgb_buffer.data(), target_height,
                                              target_width, target_rgba_buffer.data());

  EXPECT_EQ(result, 0);
  // Add additional assertions to verify the correctness of the scaling operation
  // ...
}

TEST(ColourSpaceCpuTest, ScaleToSizeBgra_NullPointers) {
  mediax::video::ColourSpaceCpu colourSpaceCpu;
  uint32_t source_height = 480;
  uint32_t source_width = 640;
  uint8_t source_rgb_buffer[640 * 480 * 3]; 
  uint32_t target_height = 240;
  uint32_t target_width = 320;
  uint8_t target_bgra_buffer[240 * 320 * 4]; 

  int result = colourSpaceCpu.ScaleToSizeBgra(source_height, source_width, source_rgb_buffer, target_height,
                                              target_width, target_bgra_buffer);

  EXPECT_EQ(result, 1);
}

TEST(ColourSpaceCpuTest, ScaleToSizeBgra_AllocationFailure) {
  mediax::video::ColourSpaceCpu colourSpaceCpu;
  uint32_t source_height = 480;
  uint32_t source_width = 640;
  std::vector<uint8_t> source_rgb_buffer(source_height * source_width * 3);
  uint32_t target_height = 240;
  uint32_t target_width = 320;
  uint8_t target_bgra_buffer[240 * 320 * 4]; ;

  int result = colourSpaceCpu.ScaleToSizeBgra(source_height, source_width, source_rgb_buffer.data(), target_height,
                                              target_width, target_bgra_buffer);

  EXPECT_EQ(result, 1);
}

TEST(ColourSpaceCpuTest, ScaleToSizeBgra_Success) {
  mediax::video::ColourSpaceCpu colourSpaceCpu;
  uint32_t source_height = 480;
  uint32_t source_width = 640;
  std::vector<uint8_t> source_rgb_buffer(source_height * source_width * 3);
  uint32_t target_height = 240;
  uint32_t target_width = 320;
  std::vector<uint8_t> target_bgra_buffer(target_height * target_width * 4);

  int result = colourSpaceCpu.ScaleToSizeBgra(source_height, source_width, source_rgb_buffer.data(), target_height,
                                              target_width, target_bgra_buffer.data());

  EXPECT_EQ(result, 0);
  // Add additional assertions to verify the correctness of the scaling operation
}