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
namespace video {

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

int ColourSpaceCpu::RgbaToRgb(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *rgb) const {
  if (!rgba || !rgb) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_RGBA, width, height, AV_PIX_FMT_RGB24, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {rgba};
  std::array<uint8_t *, 1> outData = {rgb};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 4)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 3)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::RgbaToYuv(uint32_t height, uint32_t width, uint8_t *rgba, uint8_t *yuv) const {
  if (!rgba || !yuv) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_RGBA, width, height, AV_PIX_FMT_UYVY422, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {rgba};
  std::array<uint8_t *, 1> outData = {yuv};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 4)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 2)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) const {
  if (!rgb || !yuv) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_RGB24, width, height, AV_PIX_FMT_UYVY422, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {rgb};
  std::array<uint8_t *, 1> outData = {yuv};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 3)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 2)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::RgbToMono8(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *mono8) const {
  if (!rgb || !mono8) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_RGB24, width, height, AV_PIX_FMT_GRAY8, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {rgb};
  std::array<uint8_t *, 1> outData = {mono8};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 3)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 1)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::RgbToMono16(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *mono16) const {
  if (!rgb || !mono16) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_RGB24, width, height, AV_PIX_FMT_GRAY16, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {rgb};
  std::array<uint8_t *, 1> outData = {mono16};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 3)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 2)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::RgbToBgra(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *bgra) const {
  if (!rgb || !bgra) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_RGB24, width, height, AV_PIX_FMT_BGRA, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {rgb};
  std::array<uint8_t *, 1> outData = {bgra};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 3)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 4)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::Mono8ToBgra(uint32_t width, uint32_t height, uint8_t *mono8, uint8_t *bgra) const {
  if (!bgra || !mono8) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_GRAY8, width, height, AV_PIX_FMT_BGRA, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {mono8};
  std::array<uint8_t *, 1> outData = {bgra};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 1)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 4)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::Mono16ToBgra(uint32_t width, uint32_t height, uint8_t *mono16, uint8_t *bgra) const {
  if (!bgra || !mono16) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_GRAY16, width, height, AV_PIX_FMT_BGRA, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {mono16};
  std::array<uint8_t *, 1> outData = {bgra};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 2)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 4)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::YuvToArgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *argb) const {
  if (!argb || !yuv) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_UYVY422, width, height, AV_PIX_FMT_ARGB, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::vector<uint8_t *> inData = {yuv};
  std::vector<uint8_t *> outData = {argb};

  // Use static_cast instead of C-style cast
  const std::vector<int32_t> inLinesize = {(int32_t)(width * 2)};
  std::vector<int32_t> outLinesize = {(int32_t)(width * 4)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());

  return 0;
}

int ColourSpaceCpu::RgbToRgba(uint32_t width, uint32_t height, uint8_t *rgb, uint8_t *rgba) const {
  if (!rgb || !rgba) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_RGB24, width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {rgb};
  std::array<uint8_t *, 1> outData = {rgba};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 3)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 4)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba) const {
  if (!yuv || !rgba) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_UYVY422, width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {yuv};
  std::array<uint8_t *, 1> outData = {rgba};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 2)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 4)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}
int ColourSpaceCpu::Mono8ToRgba(uint32_t width, uint32_t height, uint8_t *mono8, uint8_t *rgba) const {
  if (!rgba || !mono8) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_GRAY8, width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {mono8};
  std::array<uint8_t *, 1> outData = {rgba};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 1)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 4)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::Mono16ToRgba(uint32_t width, uint32_t height, uint8_t *mono16, uint8_t *rgba) const {
  if (!rgba || !mono16) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(width, height, AV_PIX_FMT_GRAY16, width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, nullptr, nullptr,
                     nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {mono16};
  std::array<uint8_t *, 1> outData = {rgba};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(width * 2)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(width * 4)};

  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, height, outData.data(), outLinesize.data());
  return 0;
}

int ColourSpaceCpu::ScaleToSizeRgb(uint32_t source_height, uint32_t source_width, uint8_t *source_rgb_buffer,
                                   uint32_t target_height, uint32_t target_width, uint8_t *target_rgb_buffer) const {
  if (!source_rgb_buffer || !target_rgb_buffer) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(source_width, source_height, AV_PIX_FMT_RGB24, target_width, target_height, AV_PIX_FMT_RGB24,
                     SWS_BICUBIC, nullptr, nullptr, nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {source_rgb_buffer};
  std::array<uint8_t *, 1> outData = {target_rgb_buffer};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(source_width * 3)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(target_width * 3)};
  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, source_height, outData.data(), outLinesize.data());

  return 0;
}

int ColourSpaceCpu::ScaleToSizeRgba(uint32_t source_height, uint32_t source_width, uint8_t *source_rgb_buffer,
                                    uint32_t target_height, uint32_t target_width, uint8_t *target_rgba_buffer) const {
  if (!source_rgb_buffer || !target_rgba_buffer) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(source_width, source_height, AV_PIX_FMT_RGBA, target_width, target_height, AV_PIX_FMT_RGBA,
                     SWS_BICUBIC, nullptr, nullptr, nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {source_rgb_buffer};
  std::array<uint8_t *, 1> outData = {target_rgba_buffer};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(source_width * 4)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(target_width * 4)};
  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, source_height, outData.data(), outLinesize.data());

  return 0;
}

int ColourSpaceCpu::ScaleToSizeBgra(uint32_t source_height, uint32_t source_width, uint8_t *source_rgb_buffer,
                                    uint32_t target_height, uint32_t target_width, uint8_t *target_bgra_buffer) const {
  if (!source_rgb_buffer || !target_bgra_buffer) {
    // Handle null pointers gracefully
    return 1;
  }

  std::unique_ptr<SwsContext, decltype(&sws_freeContext)> ctx(
      sws_getContext(source_width, source_height, AV_PIX_FMT_BGRA, target_width, target_height, AV_PIX_FMT_BGRA,
                     SWS_BICUBIC, nullptr, nullptr, nullptr),
      &sws_freeContext);
  if (!ctx) {
    // Handle allocation failure gracefully
    return 1;
  }

  const std::array<uint8_t *, 1> inData = {source_rgb_buffer};
  std::array<uint8_t *, 1> outData = {target_bgra_buffer};

  // Use static_cast instead of C-style cast
  const std::array<int32_t, 1> inLinesize = {(int32_t)(source_width * 4)};
  std::array<int32_t, 1> outLinesize = {(int32_t)(target_width * 4)};
  sws_scale(ctx.get(), inData.data(), inLinesize.data(), 0, source_height, outData.data(), outLinesize.data());

  return 0;
}

}  // namespace video
