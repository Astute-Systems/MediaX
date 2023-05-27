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

#include "rtp_utils.h"

#include <cmath>
#include <iomanip>
#include <iostream>

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

// Implementation of the CreateColourBarTestCard function
void CreateColourBarTestCard(uint8_t *data, uint32_t width, uint32_t height) {
  for (uint32_t x = 0; x < width; x++) {
    for (uint32_t y = 0; y < height; y++) {
      uint8_t r, g, b;
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
      uint32_t index = (y * width + x) * 3;
      data[index] = r;
      data[index + 1] = g;
      data[index + 2] = b;
    }
  }
}

// Implementation of the CreateGreyScaleBarTestCard function
void CreateGreyScaleBarTestCard(uint8_t *data, uint32_t width, uint32_t height) {
  uint32_t bar_width = width / 8;
  uint32_t bar_height = height;
  uint8_t color = 0;
  for (uint32_t i = 0; i < 8; i++) {
    for (uint32_t y = 0; y < bar_height; y++) {
      for (uint32_t x = i * bar_width; x < (i + 1) * bar_width; x++) {
        data[(y * width + x) * 3] = color;      // Red channel
        data[(y * width + x) * 3 + 1] = color;  // Green channel
        data[(y * width + x) * 3 + 2] = color;  // Blue channel
      }
    }
    color += 32;
  }
}

void CreateComplexTestCard(uint8_t *data, uint32_t width, uint32_t height) {
  uint32_t size = width * height * 3;
  for (uint32_t i = 0; i < size; i += 3) {
    uint32_t x = i / 3 % width;
    uint32_t y = i / 3 / width;
    double r, g, b;
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
    double t = (double)i / size * 2 * M_PI;
    r *= 0.5 + 0.5 * sin(t);
    g *= 0.5 + 0.5 * sin(t + 2 * M_PI / 3);
    b *= 0.5 + 0.5 * sin(t + 4 * M_PI / 3);
    data[i] = (uint8_t)r;
    data[i + 1] = (uint8_t)g;
    data[i + 2] = (uint8_t)b;
  }
}

void CreateCheckeredTestCard(uint8_t *data, uint32_t width, uint32_t height) {
  uint32_t size = width * height * 3;
  for (uint32_t i = 0; i < size; i += 3) {
    uint32_t x = i / 3 % width;
    uint32_t y = i / 3 / width;
    uint8_t r, g, b;
    if ((x / 8 + y / 8) % 2 == 0) {
      r = 255;
      g = 255;
      b = 255;
    } else {
      r = 0;
      g = 0;
      b = 0;
    }
    data[i] = r;
    data[i + 1] = g;
    data[i + 2] = b;
  }
}