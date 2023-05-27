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
  uint32_t x, y;
  uint8_t r, g, b;

  // Iterate over each pixel in the image
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      // Set the RGB values of the pixel based on its position in the image
      if (x < width / 7) {
        r = 255;
        g = 0;
        b = 0;
      } else if (x < 2 * width / 7) {
        r = 255;
        g = 255;
        b = 0;
      } else if (x < 3 * width / 7) {
        r = 0;
        g = 255;
        b = 0;
      } else if (x < 4 * width / 7) {
        r = 0;
        g = 255;
        b = 255;
      } else if (x < 5 * width / 7) {
        r = 0;
        g = 0;
        b = 255;
      } else if (x < 6 * width / 7) {
        r = 255;
        g = 0;
        b = 255;
      } else {
        r = 255;
        g = 0;
        b = 0;
      }
      // Set the RGB values of the pixel in the data array
      *data++ = r;
      *data++ = g;
      *data++ = b;
    }
  }
}

// Implementation of the CreateGreyScaleBarTestCard function
void CreateGreyScaleBarTestCard(uint8_t *data, uint32_t width, uint32_t height) {
  uint32_t x, y;
  uint8_t value;

  // Iterate over each pixel in the image
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      // Set the grey scale value of the pixel based on its position in the image
      value = (x * 255) / (width - 1);
      // Set the grey scale value of the pixel in the data array
      *data++ = value;
    }
  }
}

void CreateSmtpeTestCard(uint8_t *data, uint32_t width, uint32_t height) {
  uint32_t bytes_per_pixel = 3;  // RGB
  uint32_t bytes_per_row = width * bytes_per_pixel;
  uint8_t *pixel_ptr = data;

  for (uint32_t y = 0; y < height; y++) {
    for (uint32_t x = 0; x < width; x++) {
      uint8_t r, g, b;

      if (x < width / 8) {
        // 75% color bars
        auto color_bar = (uint8_t)(x / (width / 8));
        switch (color_bar) {
          case 0:
            r = 0xFF;
            g = 0x00;
            b = 0x00;
            break;  // red
          case 1:
            r = 0xFF;
            g = 0xFF;
            b = 0x00;
            break;  // yellow
          case 2:
            r = 0x00;
            g = 0xFF;
            b = 0x00;
            break;  // green
          case 3:
            r = 0x00;
            g = 0xFF;
            b = 0xFF;
            break;  // cyan
          case 4:
            r = 0x00;
            g = 0x00;
            b = 0xFF;
            break;  // blue
          case 5:
            r = 0xFF;
            g = 0x00;
            b = 0xFF;
            break;  // magenta
          case 6:
            r = 0x80;
            g = 0x80;
            b = 0x80;
            break;  // gray
          default:
            r = 0x00;
            g = 0x00;
            b = 0x00;
            break;
        }
      } else if (x < width / 4) {
        // 100% color bars
        auto color_bar = (uint8_t)(x - width / 8) / (width / 16);
        switch (color_bar) {
          case 0:
            r = 0xFF;
            g = 0x00;
            b = 0x00;
            break;  // red
          case 1:
            r = 0xFF;
            g = 0xFF;
            b = 0x00;
            break;  // yellow
          case 2:
            r = 0x00;
            g = 0xFF;
            b = 0x00;
            break;  // green
          case 3:
            r = 0x00;
            g = 0xFF;
            b = 0xFF;
            break;  // cyan
          case 4:
            r = 0x00;
            g = 0x00;
            b = 0xFF;
            break;  // blue
          case 5:
            r = 0xFF;
            g = 0x00;
            b = 0xFF;
            break;  // magenta
          case 6:
            r = 0x80;
            g = 0x80;
            b = 0x80;
            break;  // gray
          case 8:
            r = 0xFF;
            g = 0xFF;
            b = 0xFF;
            break;  // white
          default:
            r = 0x00;
            g = 0x00;
            b = 0x00;
            break;
        }
      } else if (x < width / 2) {
        // 100% bars with pluge
        auto color_bar = (uint8_t)(x - width / 4) / (width / 16);
        switch (color_bar) {
          default:
            r = 0x00;
            g = 0x00;
            b = 0x00;
            break;
        }
      } else {
        // 100% bars with pluge and color bars
        auto color_bar = (uint8_t)(x - width / 2) / (width / 16);

        if (y < height / 3) {
          // 100% color bars
          switch (color_bar) {
            case 0:
              r = 0xFF;
              g = 0x00;
              b = 0x00;
              break;  // red
            case 1:
              r = 0xFF;
              g = 0xFF;
              b = 0x00;
              break;  // yellow
            case 2:
              r = 0x00;
              g = 0xFF;
              b = 0x00;
              break;  // green
            case 3:
              r = 0x00;
              g = 0xFF;
              b = 0xFF;
              break;  // cyan
            case 4:
              r = 0x00;
              g = 0x00;
              b = 0xFF;
              break;  // blue
            case 5:
              r = 0xFF;
              g = 0x00;
              b = 0xFF;
              break;  // magenta
            case 6:
              r = 0x80;
              g = 0x80;
              b = 0x80;
              break;  // gray
            case 8:
              r = 0xFF;
              g = 0xFF;
              b = 0xFF;
              break;  // white
            default:
              r = 0x00;
              g = 0x00;
              b = 0x00;
              break;
          }
        } else if (y < height * 2 / 3) {
          // pluge
          switch (color_bar) {
            case 0:
              r = 0x10;
              g = 0x10;
              b = 0x10;
              break;  // black
            case 1:
              r = 0x20;
              g = 0x20;
              b = 0x20;
              break;  // 3.5%
            case 2:
              r = 0x30;
              g = 0x30;
              b = 0x30;
              break;  // 7%
            case 3:
              r = 0x40;
              g = 0x40;
              b = 0x40;
              break;  // 10.5%
            case 4:
              r = 0x50;
              g = 0x50;
              b = 0x50;
              break;  // 14%
            case 5:
              r = 0x60;
              g = 0x60;
              b = 0x60;
              break;  // 17.5%
            case 6:
              r = 0x70;
              g = 0x70;
              b = 0x70;
              break;  // 21%
            case 7:
              r = 0x80;
              g = 0x80;
              b = 0x80;
              break;  // 24.5%
            case 8:
              r = 0x90;
              g = 0x90;
              b = 0x90;
              break;  // 28%
            case 9:
              r = 0xA0;
              g = 0xA0;
              b = 0xA0;
              break;  // 31.5%
            case 10:
              r = 0xB0;
              g = 0xB0;
              b = 0xB0;
              break;  // 35%
            case 11:
              r = 0xC0;
              g = 0xC0;
              b = 0xC0;
              break;  // 38.5%
            case 12:
              r = 0xD0;
              g = 0xD0;
              b = 0xD0;
              break;  // 42%
            case 13:
              r = 0xE0;
              g = 0xE0;
              b = 0xE0;
              break;  // 45.5%
            case 14:
              r = 0xF0;
              g = 0xF0;
              b = 0xF0;
              break;  // 49%
            case 15:
              r = 0xFF;
              g = 0xFF;
              b = 0xFF;
              break;  // white
            default:
              r = 0x00;
              g = 0x00;
              b = 0x00;
              break;
          }
        } else {
          // 100% color bars
          switch (color_bar) {
            case 0:
              r = 0xFF;
              g = 0x00;
              b = 0x00;
              break;  // red
            case 1:
              r = 0xFF;
              g = 0xFF;
              b = 0x00;
              break;  // yellow
            case 2:
              r = 0x00;
              g = 0xFF;
              b = 0x00;
              break;  // green
            case 3:
              r = 0x00;
              g = 0xFF;
              b = 0xFF;
              break;  // cyan
            case 4:
              r = 0x00;
              g = 0x00;
              b = 0xFF;
              break;  // blue
            case 5:
              r = 0xFF;
              g = 0x00;
              b = 0xFF;
              break;  // magenta
            case 6:
              r = 0x80;
              g = 0x80;
              b = 0x80;
              break;  // gray
            case 7:
              r = 0x00;
              g = 0x00;
              b = 0x00;
              break;  // black
            case 8:
              r = 0xFF;
              g = 0xFF;
              b = 0xFF;
              break;  // white
            default:
              r = 0x00;
              g = 0x00;
              b = 0x00;
              break;
          }
        }
      }

      *pixel_ptr++ = r;
      *pixel_ptr++ = g;
      *pixel_ptr++ = b;
    }
    pixel_ptr += bytes_per_row - bytes_per_pixel * width;
  }
}
