//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP utility functions
///
/// \file rtp_utils.cc

#include "rtp/rtp_utils.h"

#include "rtp/rtp_types.h"

#if GST_SUPPORTED
#include <gst/gst.h>
#endif

#include <glog/logging.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <thread>

/// The DefenceX (MediaX) library for video streaming
namespace mediax {

void InitRtp(int argc, char *argv[]) {
#if GST_SUPPORTED
  // Initialize GStreamer
  gst_init(&argc, &argv);
#endif
}

bool IsRtpInitialised() {
#if GST_SUPPORTED
  return gst_is_initialized();
#else
  return false;
#endif
}

void RtpCleanup() {
#if GST_SUPPORTED
  // Deinitialize GStreamer
  gst_deinit();
#endif
}

std::string ColourspaceTypeToString(rtp::ColourspaceType) {
  switch (rtp::ColourspaceType::kColourspaceRgba) {
    case rtp::ColourspaceType::kColourspaceRgba:
      return "RGBA";
    case rtp::ColourspaceType::kColourspaceRgb24:
      return "RGB24";
    case rtp::ColourspaceType::kColourspaceYuv:
      return "YUV";
    case rtp::ColourspaceType::kColourspaceMono16:
      return "MONO16";
    case rtp::ColourspaceType::kColourspaceMono8:
      return "MONO8";
    case rtp::ColourspaceType::kColourspaceNv12:
      return "NV12";
    case rtp::ColourspaceType::kColourspaceH264Part4:
      return "H264Part4";
    case rtp::ColourspaceType::kColourspaceH264Part10:
      return "H264Part10";
    case rtp::ColourspaceType::kColourspaceH265:
      return "H265";
    default:
      return "Unknown";
  }
}

rtp::ColourspaceType ColourspaceTypeFromString(std::string_view str) {
  if (str == "RGBA") {
    return rtp::ColourspaceType::kColourspaceRgba;
  } else if (str == "RGB24") {
    return rtp::ColourspaceType::kColourspaceRgb24;
  } else if (str == "YUV") {
    return rtp::ColourspaceType::kColourspaceYuv;
  } else if (str == "MONO16") {
    return rtp::ColourspaceType::kColourspaceMono16;
  } else if (str == "MONO8") {
    return rtp::ColourspaceType::kColourspaceMono8;
  } else if (str == "NV12") {
    return rtp::ColourspaceType::kColourspaceNv12;
  } else if (str == "H264Part4") {
    return rtp::ColourspaceType::kColourspaceH264Part4;
  } else if (str == "H264Part10") {
    return rtp::ColourspaceType::kColourspaceH264Part10;
  } else if (str == "H265") {
    return rtp::ColourspaceType::kColourspaceH265;
  } else {
    return rtp::ColourspaceType::kColourspaceUndefined;
  }
}

uint8_t BitsPerPixel(rtp::ColourspaceType mode) {
  switch (mode) {
    case rtp::ColourspaceType::kColourspaceRgba:
      return 32;
    case rtp::ColourspaceType::kColourspaceRgb24:
      return 24;
    case rtp::ColourspaceType::kColourspaceYuv:
      return 16;
    case rtp::ColourspaceType::kColourspaceMono16:
      return 16;
    case rtp::ColourspaceType::kColourspaceMono8:
      return 8;
    case rtp::ColourspaceType::kColourspaceNv12:
      return 12;
    case rtp::ColourspaceType::kColourspaceH264Part4:
    case rtp::ColourspaceType::kColourspaceH264Part10:
    case rtp::ColourspaceType::kColourspaceH265:
      return 24;  // Not known but not an error.
    case rtp::ColourspaceType::kColourspaceJpeg2000:
      return 24;  // Not known but not an error.
    case rtp::ColourspaceType::kColourspaceAv1:
      return 24;  // Not known but not an error.
    default:
      LOG(ERROR) << "Unknown colourspace type " << ColourspaceTypeToString(mode);
      return 0;
  }
}
uint8_t BytesPerPixel(rtp::ColourspaceType mode) {
  // Round up to next byte
  return static_cast<uint8_t>(std::ceil(BitsPerPixel(mode) / static_cast<double>(8)));
}
}  // namespace mediax

void EndianSwap32(uint32_t *data, unsigned int length) {
  // Check for Intel architecture
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#ifdef _WIN32
  for (uint32_t c = 0; c < length; c++) data[c] = _byteswap_ulong(data[c]);
#else
  for (uint32_t c = 0; c < length; c++) data[c] = __bswap_32(data[c]);
#endif
  return;
// Check for ARM architecture
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return;
#else
#error "Unknown byte order"
#endif
}

void EndianSwap16(uint16_t *data, unsigned int length) {
  // Check for Intel architecture
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  for (uint16_t c = 0; c < length; c++) data[c] = __bswap_16(data[c]);
  return;
// Check for ARM architecture
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  return;
#else
#error "Unknown byte order"
#endif
}

void DumpHex(const void *data, size_t size) {
  const auto p = static_cast<const unsigned char *>(data);
  std::cout << std::hex << std::setfill('0');
  for (size_t i = 0; i < size; ++i) {
    std::cout << std::setw(2) << static_cast<unsigned int>(p[i]) << ' ';
    if ((i + 1) % 16 == 0) {
      std::cout << "  ";
      for (size_t j = i - 15; j <= i; ++j) {
        if (std::isprint(p[j])) {
          std::cout << static_cast<char>(p[j]);
        } else {
          std::cout << '.';
        }
      }
      std::cout << std::endl;
    }
  }
  if (size % 16 != 0) {
    size_t remaining = size % 16;
    for (size_t i = 0; i < 16 - remaining; ++i) {
      std::cout << "   ";
    }
    std::cout << "  ";
    for (size_t i = size - remaining; i < size; ++i) {
      if (std::isprint(p[i])) {
        std::cout << static_cast<char>(p[i]);
      } else {
        std::cout << '.';
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::dec << std::endl;
}

///
/// \brief Pack Red/Green/Blue values into a buffer
///
/// \param data the buffer to pack into
/// \param r red value
/// \param g green value
/// \param b blue value
/// \param colourspace the colourspace to use
///
void PackRgb(uint8_t *data, uint32_t r, uint32_t g, uint32_t b, mediax::rtp::ColourspaceType colourspace) {
  static bool odd = true;
  switch (colourspace) {
    default:
      data[0] = (uint8_t)r;
      data[1] = (uint8_t)g;
      data[2] = (uint8_t)b;
      break;
    case mediax::rtp::ColourspaceType::kColourspaceYuv: {
      double y = 0.257 * r + 0.504 * g + 0.098 * b + 16;
      if (odd) {
        double u = -0.148 * r - 0.291 * g + 0.439 * b + 128;
        data[0] = (uint8_t)(u);
        odd = false;
      } else {
        double v = 0.439 * r - 0.368 * g - 0.071 * b + 128;
        data[0] = (uint8_t)(v);
        odd = true;
      }
      data[1] = (uint8_t)(y);
    } break;
    case mediax::rtp::ColourspaceType::kColourspaceRgba:
      data[0] = (uint8_t)r;
      data[1] = (uint8_t)g;
      data[2] = (uint8_t)b;
      data[3] = 0;  // Alpha
      break;
    case mediax::rtp::ColourspaceType::kColourspaceMono8:
      data[0] = (uint8_t)(0.299 * r + 0.587 * g + 0.114 * b);
      break;
    case mediax::rtp::ColourspaceType::kColourspaceMono16: {
      auto mono16_pixel = (uint16_t)(0.299 * r + 0.587 * g + 0.114 * b);
      data[0] = mono16_pixel >> 8 & 0xFF;
      data[1] = mono16_pixel & 0xFF;
    } break;
  }
}

// Implementation of the CreateColourBarTestCard function
void CreateColourBarEbuTestCard(uint8_t *data, uint32_t width, uint32_t height,
                                mediax::rtp::ColourspaceType colourspace) {
  uint32_t stride = mediax::BytesPerPixel(colourspace);
  for (uint32_t y = 0; y < height; y++) {
    for (uint32_t x = 0; x < width; x++) {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      // Set the color of each pixel based on its position in the image.
      if (x < width / 8) {
        // white
        r = 255;
        g = 255;
        b = 255;
      } else if (x < width * 2 / 8) {
        // yellow
        r = 255;
        g = 255;
        b = 0;
      } else if (x < width * 3 / 8) {
        // cyan
        r = 0;
        g = 255;
        b = 255;
      } else if (x < width * 4 / 8) {
        // green
        r = 0;
        g = 255;
        b = 0;
      } else if (x < width * 5 / 8) {
        // Magenta
        r = 255;
        g = 0;
        b = 255;
      } else if (x < width * 6 / 8) {
        // red
        r = 255;
        g = 0;
        b = 0;
      } else if (x < width * 7 / 8) {
        // blue
        r = 0;
        g = 0;
        b = 255;
      } else {
        // black
        r = 0;
        g = 0;
        b = 0;
      }
      // Set the color of the current pixel in the image data buffer.
      uint32_t index = (y * width + x) * stride;
      PackRgb(&data[index], r, g, b, colourspace);
    }
  }
}

// Implementation of the CreateColourBarTestCard function
void CreateColourBarTestCard(uint8_t *data, uint32_t width, uint32_t height, mediax::rtp::ColourspaceType colourspace) {
  uint32_t stride = mediax::BytesPerPixel(colourspace);
  for (uint32_t y = 0; y < height; y++) {
    for (uint32_t x = 0; x < width; x++) {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      // Set the color of each pixel based on its position in the image.
      if (x < width / 8) {
        r = 255;
        g = 0;
        b = 0;
      } else if (x < width * 2 / 8) {
        r = 255;
        g = 127;
        b = 0;
      } else if (x < width * 3 / 8) {
        r = 255;
        g = 255;
        b = 0;
      } else if (x < width * 4 / 8) {
        r = 0;
        g = 255;
        b = 0;
      } else if (x < width * 5 / 8) {
        r = 0;
        g = 255;
        b = 255;
      } else if (x < width * 6 / 8) {
        r = 0;
        g = 0;
        b = 255;
      } else if (x < width * 7 / 8) {
        r = 127;
        g = 0;
        b = 255;
      } else {
        r = 255;
        g = 0;
        b = 255;
      }
      // Set the color of the current pixel in the image data buffer.
      uint32_t index = (y * width + x) * stride;
      PackRgb(&data[index], r, g, b, colourspace);
    }
  }
}

// Implementation of the CreateGreyScaleBarTestCard function
void CreateGreyScaleBarTestCard(uint8_t *data, uint32_t width, uint32_t height,
                                mediax::rtp::ColourspaceType colourspace) {
  uint32_t stride = mediax::BytesPerPixel(colourspace);

  uint32_t bar_width = width / 8;
  uint32_t bar_height = height;
  uint8_t color = 0;
  for (uint32_t i = 0; i < 8; i++) {
    for (uint32_t y = 0; y < bar_height; y++) {
      for (uint32_t x = i * bar_width; x < (i + 1) * bar_width; x++) {
        PackRgb(&data[(y * width + x) * stride], color, color, color, colourspace);  // Blue channel
      }
    }
    color += 32;
  }
}

void CreateQuadTestCard(uint8_t *data, uint32_t width, uint32_t height, mediax::rtp::ColourspaceType colourspace) {
  uint32_t stride = mediax::BytesPerPixel(colourspace);

  uint32_t size = width * height * stride;
  for (uint32_t i = 0; i < size; i += stride) {
    uint32_t x = i / stride % width;
    uint32_t y = i / stride / width;
    double r;
    double g;
    double b;
    if (x < width / 2 && y < height / 2) {
      r = 0;
      g = 0;
      b = 0;
    } else if (x >= width / 2 && y < height / 2) {
      r = 255;
      g = 0;
      b = 0;
    } else if (x < width / 2 && y >= height / 2) {
      r = 0;
      g = 255;
      b = 0;
    } else {
      r = 0;
      g = 0;
      b = 255;
    }

    PackRgb(&data[i], static_cast<uint32_t>(r), static_cast<uint32_t>(g), static_cast<uint32_t>(b), colourspace);
  }
}

void CreateCheckeredTestCard(uint8_t *data, uint32_t width, uint32_t height, mediax::rtp::ColourspaceType colourspace) {
  uint32_t stride = mediax::BytesPerPixel(colourspace);

  for (uint32_t y = 0; y < height; y++) {
    for (uint32_t x = 0; x < width; x++) {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      if ((x / 8 + y / 8) % 2 == 0) {
        r = 255;
        g = 255;
        b = 255;
      } else {
        r = 0;
        g = 0;
        b = 0;
      }
      uint32_t index = (y * width + x) * stride;
      PackRgb(&data[index], r, g, b, colourspace);
    }
  }
}

void CreateSolidTestCard(uint8_t *data, uint32_t width, uint32_t height, uint8_t red, uint8_t green, uint8_t blue,
                         mediax::rtp::ColourspaceType colourspace) {
  uint32_t stride = mediax::BytesPerPixel(colourspace);

  uint32_t size = width * height * stride;
  for (uint32_t i = 0; i < size; i += stride) {
    PackRgb(&data[i], red, green, blue, colourspace);
  }
}

void CreateWhiteNoiseTestCard(uint8_t *data, uint32_t width, uint32_t height,
                              mediax::rtp::ColourspaceType colourspace) {
  uint32_t stride = mediax::BytesPerPixel(colourspace);

  uint32_t size = width * height * stride;

  for (uint32_t i = 0; i < size; i += stride) {
    // random pixel 0 or 255
    uint8_t pixel = rand() % 2 == 0 ? 0 : 255;  // NOLINT
    uint8_t r = pixel;
    uint8_t g = pixel;
    uint8_t b = pixel;
    PackRgb(&data[i], r, g, b, colourspace);
  }
}

struct Ball {
  float x;   // position
  float y;   // position
  float vx;  // velocity
  float vy;  // velocity
};

void CreateBouncingBallTestCard(uint8_t *data, uint32_t width, uint32_t height,
                                mediax::rtp::ColourspaceType colourspace) {
  int ball_size = 50;
  int half = ball_size / 2;
  static Ball ball = {static_cast<float>(width) / 2, static_cast<float>(height) / 2, 5,
                      5};  // start in the middle of the screen, moving diagonally

  // Update ball position
  ball.x += ball.vx;
  ball.y += ball.vy;

  // Bounce off edges, ball is the width defined by size
  if (ball.x < static_cast<float>(ball_size + 1) || ball.x > static_cast<float>(width) - ball_size) {
    ball.vx *= -1;
  }
  if (ball.y < static_cast<float>(ball_size + 1) || ball.y > static_cast<float>(height) - ball_size) {
    ball.vy *= -1;
  }

  // Ensure ball stays within screen bounds (it might have gone past due to the velocity)
  ball.x = std::clamp(ball.x, 0.0f, static_cast<float>(width - ball_size));
  ball.y = std::clamp(ball.y, 0.0f, static_cast<float>(height - ball_size));

  // Draw ball on test card
  // This depends on how your test card is represented in memory
  // For example, if it's a simple RGB image, you might do:
  int index = static_cast<int>(ball.y) * width * 3 + static_cast<int>(ball.x) * 3;

  // Set the background to black
  for (uint32_t i = 0; i < width * height * 3; i += 3) {
    PackRgb(&data[i], 0, 0, 0, colourspace);
  }

  // Draw the circle 50 pixels wide
  for (int y = -half; y < half; y++) {
    for (int x = -half; x < half; x++) {
      // Only draw pixels within the circle
      if (x * x + y * y < half * half) {
        // Set the pixel to white
        PackRgb(&data[index + (y * width + x) * 3], 255, 255, 255, colourspace);
      }
    }
  }
}
