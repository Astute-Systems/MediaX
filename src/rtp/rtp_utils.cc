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

#include <cmath>
#include <iomanip>
#include <iostream>

namespace mediax {

void InitRtp(int argc, char *argv[]) {
#if GST_SUPPORTED
  // Initialize GStreamer
  gst_init(&argc, &argv);
#endif
}

void RtpCleanup() {
#if GST_SUPPORTED
  // Deinitialize GStreamer
  gst_deinit();
#endif
}

uint8_t BitsPerPixel(ColourspaceType mode) {
  switch (mode) {
    case ColourspaceType::kColourspaceRgba:
      return 32;
    case ColourspaceType::kColourspaceRgb24:
      return 24;
    case ColourspaceType::kColourspaceYuv:
      return 16;
    case ColourspaceType::kColourspaceMono16:
      return 16;
    case ColourspaceType::kColourspaceMono8:
      return 8;
    default:
      LOG(ERROR) << "Unknown colourspace type";
      return 0;
  }
}
uint8_t BytesPerPixel(ColourspaceType mode) { return BitsPerPixel(mode) / 8; }
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

static bool odd = true;

void PackRgb(uint8_t *data, uint32_t r, uint32_t g, uint32_t b, mediax::ColourspaceType colourspace) {
  switch (colourspace) {
    case mediax::ColourspaceType::kColourspaceYuv: {
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
    case mediax::ColourspaceType::kColourspaceRgb24:
      data[0] = (uint8_t)r;
      data[1] = (uint8_t)g;
      data[2] = (uint8_t)b;
      break;
    case mediax::ColourspaceType::kColourspaceRgba:
      data[0] = (uint8_t)r;
      data[1] = (uint8_t)g;
      data[2] = (uint8_t)b;
      data[3] = 0;  // Alpha
      break;
    case mediax::ColourspaceType::kColourspaceMono8:
      data[0] = (uint8_t)(0.299 * r + 0.587 * g + 0.114 * b);
      break;
    case mediax::ColourspaceType::kColourspaceMono16: {
      uint16_t mono16_pixel = (uint16_t)(0.299 * r + 0.587 * g + 0.114 * b);
      data[0] = mono16_pixel >> 8 & 0xFF;
      data[1] = mono16_pixel & 0xFF;
    } break;
    default:
      break;
  }
}

// Implementation of the CreateColourBarTestCard function
void CreateColourBarTestCard(uint8_t *data, uint32_t width, uint32_t height, mediax::ColourspaceType colourspace) {
  uint32_t stride = mediax::BytesPerPixel(colourspace);
  odd = true;
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
void CreateGreyScaleBarTestCard(uint8_t *data, uint32_t width, uint32_t height, mediax::ColourspaceType colourspace) {
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

void CreateQuadTestCard(uint8_t *data, uint32_t width, uint32_t height, mediax::ColourspaceType colourspace) {
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

    PackRgb(&data[i], r, g, b, colourspace);
  }
}

void CreateCheckeredTestCard(uint8_t *data, uint32_t width, uint32_t height, mediax::ColourspaceType colourspace) {
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
                         mediax::ColourspaceType colourspace) {
  uint32_t stride = mediax::BytesPerPixel(colourspace);

  uint32_t size = width * height * stride;
  for (uint32_t i = 0; i < size; i += stride) {
    PackRgb(&data[i], red, green, blue, colourspace);
  }
}
