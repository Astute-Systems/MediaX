//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief Functions to convert between different colour spaces
///
/// \file colourspace_cpu.cc
///

#include "utils/colourspace_cpu.h"

#include <array>
#include <memory>
#include <vector>

extern "C" {
#include "libswscale/swscale.h"
}
namespace mediax::video {

int ColourSpaceCpu::YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) const {
  if (!rgb || !yuv) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_UYVY422, width, height, AV_PIX_FMT_RGB24, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {yuv};
  std::array<uint8_t *, 1> outData = {rgb};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 2)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 3)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::YuvToBgra(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *gbra) const {
  if (!gbra || !yuv) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_UYVY422, width, height, AV_PIX_FMT_BGRA, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {yuv};
  std::array<uint8_t *, 1> outData = {gbra};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 2)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 4)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::Convert(uint32_t width, uint32_t height, uint8_t *in, AVPixelFormat in_format, uint8_t in_bytes,
                            uint8_t *out, AVPixelFormat out_format, uint8_t out_bytes) const {
  if (!in || !out) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, in_format, width, height, out_format, SWS_BICUBIC, nullptr, nullptr, nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {in};
  std::array<uint8_t *, 1> outData = {out};

  // Bits use for AV_PIX_FMT_RGB24
  int bits = 24;
  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * in_bytes)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * out_bytes)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::RgbaToRgb(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *rgb) const {
  return Convert(width, height, rgba, AV_PIX_FMT_RGBA, 4, rgb, AV_PIX_FMT_RGB24, 3);
}

int ColourSpaceCpu::RgbaToYuv(uint32_t height, uint32_t width, uint8_t *rgba, uint8_t *yuv) const {
  return Convert(width, height, rgba, AV_PIX_FMT_RGBA, 4, yuv, AV_PIX_FMT_UYVY422, 2);
}

int ColourSpaceCpu::RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) const {
  return Convert(width, height, rgb, AV_PIX_FMT_RGB24, 4, yuv, AV_PIX_FMT_UYVY422, 2);
}

int ColourSpaceCpu::RgbToMono8(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *mono8) const {
  return Convert(width, height, rgb, AV_PIX_FMT_RGB24, 3, mono8, AV_PIX_FMT_GRAY8, 1);
}

int ColourSpaceCpu::RgbToMono16(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *mono16) const {
  return Convert(width, height, rgb, AV_PIX_FMT_RGB24, 3, mono16, AV_PIX_FMT_GRAY16, 2);
}

int ColourSpaceCpu::RgbToBgra(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *bgra) const {
  return Convert(width, height, rgb, AV_PIX_FMT_RGB24, 3, bgra, AV_PIX_FMT_BGRA, 4);
}

int ColourSpaceCpu::Mono8ToBgra(uint32_t width, uint32_t height, uint8_t *mono8, uint8_t *bgra) const {
  return Convert(width, height, mono8, AV_PIX_FMT_GRAY8, 1, bgra, AV_PIX_FMT_BGRA, 4);
}

int ColourSpaceCpu::Mono16ToBgra(uint32_t width, uint32_t height, uint8_t *mono16, uint8_t *bgra) const {
  return Convert(width, height, mono16, AV_PIX_FMT_GRAY16, 2, bgra, AV_PIX_FMT_BGRA, 4);
}

int ColourSpaceCpu::Nv12ToBgra(uint32_t height, uint32_t width, uint8_t *nv12, uint8_t *bgra) const {
  return Convert(width, height, nv12, AV_PIX_FMT_NV12, 1, bgra, AV_PIX_FMT_BGRA, 4);
}

int ColourSpaceCpu::Nv12ToRgb(uint32_t height, uint32_t width, uint8_t *nv12, uint8_t *rgb) const {
  return Convert(width, height, nv12, AV_PIX_FMT_NV12, 1, rgb, AV_PIX_FMT_RGB24, 3);
}

int ColourSpaceCpu::YuvToArgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *argb) const {
  return Convert(width, height, yuv, AV_PIX_FMT_UYVY422, 2, argb, AV_PIX_FMT_ARGB, 4);
}

int ColourSpaceCpu::RgbToRgba(uint32_t width, uint32_t height, uint8_t *rgb, uint8_t *rgba) const {
  return Convert(width, height, rgb, AV_PIX_FMT_RGB24, 3, rgba, AV_PIX_FMT_RGBA, 4);
}

int ColourSpaceCpu::YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba) const {
  return Convert(width, height, yuv, AV_PIX_FMT_UYVY422, 2, rgba, AV_PIX_FMT_RGBA, 4);
}

int ColourSpaceCpu::Mono8ToRgba(uint32_t width, uint32_t height, uint8_t *mono8, uint8_t *rgba) const {
  return Convert(width, height, mono8, AV_PIX_FMT_GRAY8, 1, rgba, AV_PIX_FMT_RGBA, 4);
}

int ColourSpaceCpu::Mono8ToRgb(uint32_t width, uint32_t height, uint8_t *mono8, uint8_t *rgb) const {
  return Convert(width, height, mono8, AV_PIX_FMT_GRAY8, 1, rgb, AV_PIX_FMT_RGB24, 3);
}

int ColourSpaceCpu::Mono16ToRgba(uint32_t width, uint32_t height, uint8_t *mono16, uint8_t *rgba) const {
  return Convert(width, height, mono16, AV_PIX_FMT_GRAY16, 2, rgba, AV_PIX_FMT_RGBA, 4);
}

int ColourSpaceCpu::ScaleToSizeRgb(uint32_t source_height, uint32_t source_width, uint8_t *source_rgb_buffer,
                                   uint32_t target_height, uint32_t target_width, uint8_t *target_rgb_buffer) const {
  return Convert(source_width, source_height, source_rgb_buffer, AV_PIX_FMT_RGB24, 3, target_rgb_buffer,
                 AV_PIX_FMT_RGB24, 3);
}

int ColourSpaceCpu::ScaleToSizeRgba(uint32_t source_height, uint32_t source_width, uint8_t *source_rgb_buffer,
                                    uint32_t target_height, uint32_t target_width, uint8_t *target_rgba_buffer) const {
  return Convert(source_width, source_height, source_rgb_buffer, AV_PIX_FMT_RGB24, 3, target_rgba_buffer,
                 AV_PIX_FMT_RGBA, 4);
}

int ColourSpaceCpu::ScaleToSizeBgra(uint32_t source_height, uint32_t source_width, uint8_t *source_rgb_buffer,
                                    uint32_t target_height, uint32_t target_width, uint8_t *target_bgra_buffer) const {
  return Convert(source_width, source_height, source_rgb_buffer, AV_PIX_FMT_RGB24, 3, target_bgra_buffer,
                 AV_PIX_FMT_BGRA, 4);
}

}  // namespace mediax::video
