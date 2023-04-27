//
// Copyright (C) 2023 DefenceX PTY LTD. All rights reserved.
//
// This software is distributed under the included copyright license.
// Any redistribution or reproduction, in part or in whole, in any form or medium, is strictly prohibited without the
// prior written consent of DefenceX PTY LTD.
//
// For any inquiries or concerns, please contact:
// DefenceX PTY LTD
// Email: enquiries@defencex.ai
//
///
///
/// \file colourspace.cc
///

#include "colourspace.h"

#include <memory>
#include <vector>
extern "C" {
#include "libswscale/swscale.h"
}
void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) {
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

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), SWS_BICUBIC, height, outData.data(), outLinesize.data());
}

void YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba) {
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

void RgbaToYuv(uint32_t height, uint32_t width, uint8_t *rgba, uint8_t *yuv) {
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

void RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) {
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