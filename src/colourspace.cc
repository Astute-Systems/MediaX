//
// MIT License
//
// Copyright (c) 2023 DefenceX (enquiries@defencex.ai)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the 'Software'), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
// THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
}

void YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) {
  if (!rgb || !yuv) {
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

  const std::array<uint8_t *, 1> inData = {rgb};
  std::array<uint8_t *, 1> outData = {yuv};

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
      sws_getContext(width, height, AV_PIX_FMT_RGBA, width, height, AV_PIX_FMT_YUYV422, 0, nullptr, nullptr, nullptr),
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