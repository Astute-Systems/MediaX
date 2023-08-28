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
/// \file colourspace.cc
///

#include "utils/colourspace.h"

#include <array>
#include <memory>
#include <vector>

extern "C" {
#include "libswscale/swscale.h"
}
namespace video {

ColourSpace::ColourSpace() { av_log_set_level(AV_LOG_ERROR); }

void ColourSpace::YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) const {
  if (!rgb || !yuv) {
    // Handle null pointers gracefully
    return;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_YUYV422, width, height, AV_PIX_FMT_RGB24, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return;
  }

  const std::array<uint8_t *, 1> inData = {yuv};
  std::array<uint8_t *, 1> outData = {rgb};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 2)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 3)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
}

void ColourSpace::YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba) const {
  if (!rgba || !yuv) {
    // Handle null pointers gracefully
    return;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_UYVY422, width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return;
  }

  const std::array<uint8_t *, 1> inData = {yuv};
  std::array<uint8_t *, 1> outData = {rgba};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 2)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 4)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
}

void ColourSpace::RgbaToRgb(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *rgb) const {
  if (!rgba || !rgb) {
    // Handle null pointers gracefully
    return;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_RGBA, width, height, AV_PIX_FMT_RGB24, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return;
  }

  const std::array<uint8_t *, 1> inData = {rgba};
  std::array<uint8_t *, 1> outData = {rgb};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 4)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 3)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
}

void ColourSpace::RgbaToYuv(uint32_t height, uint32_t width, uint8_t *rgba, uint8_t *yuv) const {
  if (!rgba || !yuv) {
    // Handle null pointers gracefully
    return;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_RGBA, width, height, AV_PIX_FMT_YUYV422, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return;
  }

  const std::array<uint8_t *, 1> inData = {rgba};
  std::array<uint8_t *, 1> outData = {yuv};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 4)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 2)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
}

void ColourSpace::RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) const {
  if (!rgb || !yuv) {
    // Handle null pointers gracefully
    return;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_RGB24, width, height, AV_PIX_FMT_YUYV422, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return;
  }

  const std::array<uint8_t *, 1> inData = {rgb};
  std::array<uint8_t *, 1> outData = {yuv};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 3)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 2)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
}

}  // namespace video