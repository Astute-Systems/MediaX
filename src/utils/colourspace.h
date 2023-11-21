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
/// \file colourspace.h
///

#ifndef UTILS_COLOURSPACE_H_
#define UTILS_COLOURSPACE_H_

#include <stdint.h>

namespace mediax::video {

/// Helper functions for different colour space options, base class
class ColourSpace {
 public:
  ///
  /// \brief Construct a new Colour Space object
  ///
  ///
  ColourSpace() = default;

  ///
  /// \brief Destroy the Colour Space object
  ///
  ///
  virtual ~ColourSpace() = default;

  ///
  /// \brief Convert YUV to RGBA
  ///
  /// \param height The height of the image
  /// \param width The width of the image
  /// \param rgb The RGB image buffer
  /// \param yuv The YUV:422 image buffer
  ///
  virtual int RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) const = 0;

  ///
  /// \brief Convert RGB to Monochrome 8 bits ber pixel
  ///
  /// \param height The height of the image
  /// \param width The width of the image
  /// \param rgb The RGB image buffer
  /// \param mono8 The monochrome 8 image buffer
  ///
  virtual int RgbToMono8(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *mono8) const = 0;

  ///
  /// \brief Convert RGB to Monochrome 16 bits ber pixel
  ///
  /// \param height  The height of the image
  /// \param width The width of the image
  /// \param rgb  The RGB image buffer
  /// \param mono16 The monochrome 16 image buffer
  ///
  virtual int RgbToMono16(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *mono16) const = 0;

  ///
  /// \brief Convert RGBA to YUV
  ///
  /// \param width The width of the image
  /// \param height The height of the image
  /// \param rgba The RGB image buffer
  /// \param yuv The YUV:422 image buffer
  ///
  virtual int RgbaToYuv(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *yuv) const = 0;

  ///
  /// \brief Convert RGBA to RGB
  ///
  /// \param width The height of the image
  /// \param height The width of the image
  /// \param rgba The RGBA image buffer
  /// \param rgb The RGB image buffer
  ///
  virtual int RgbaToRgb(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *rgb) const = 0;

  ///
  /// \brief Convert RGB to RGBA
  ///
  /// \param width The height of the image
  /// \param height The width of the image
  /// \param rgba The RGBA image buffer
  /// \param rgb The RGB image buffer
  ///
  virtual int RgbToBgra(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *rgb) const = 0;

  ///
  /// \brief Convert YUV to RGBA
  ///
  /// \param height The height of the image
  /// \param width The width of the image
  /// \param yuv The YUV:422 image buffer
  /// \param rgba The RGBA image buffer
  ///
  virtual int YuvToBgra(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba) const = 0;

  ///
  /// \brief Convert Mono8 to RGBA on the CPU
  ///
  /// \param width  The height of the image
  /// \param height The width of the image
  /// \param mono8 The monochrome 8 bit image
  /// \param bgra The BGRA image buffer
  ///
  virtual int Mono8ToBgra(uint32_t width, uint32_t height, uint8_t *mono8, uint8_t *bgra) const = 0;

  ///
  /// \brief Convert Mono16 to RGBA on the CPU
  ///
  /// \param width The height of the image
  /// \param height The width of the image
  /// \param mono16 The monochrome 16 bit image
  /// \param bgra The BGRA image buffer
  ///
  virtual int Mono16ToBgra(uint32_t width, uint32_t height, uint8_t *mono16, uint8_t *bgra) const = 0;

  ///
  /// \brief Convert RGB to RGBA on the CPU
  ///
  /// \param width The height of the image
  /// \param height The width of the image
  /// \param rgb The RGB image buffer
  /// \param rgba The RGBA image buffer
  ///
  virtual int RgbToRgba(uint32_t width, uint32_t height, uint8_t *rgb, uint8_t *rgba) const = 0;

  ///
  /// \brief Convert YUV to RGBA on the CPU
  ///
  /// \param height The height of the image
  /// \param width The width of the image
  /// \param yuv The YUV:422 image buffer
  /// \param rgba The RGBA image buffer
  ///
  virtual int YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba) const = 0;

  ///
  /// \brief Convert Mono8 to RGBA on the CPU
  ///
  /// \param width The height of the image
  /// \param height The width of the image
  /// \param mono8 The monochrome 8 bit image
  /// \param rgba The RGBA image buffer
  ///
  virtual int Mono8ToRgba(uint32_t width, uint32_t height, uint8_t *mono8, uint8_t *rgba) const = 0;

  ///
  /// \brief Convert Mono8 to RGB on the CPU
  ///
  /// \param width The height of the image
  /// \param height The width of the image
  /// \param mono8 The monochrome 8 bit image
  /// \param rgb The RGB image buffer
  /// \return int
  ///
  virtual int Mono8ToRgb(uint32_t width, uint32_t height, uint8_t *mono8, uint8_t *rgb) const = 0;

  ///
  /// \brief Convert Mono16 to RGBA on the CPU
  ///
  /// \param width The height of the image
  /// \param height The width of the image
  /// \param mono16 The monochrome 16 bit image
  /// \param rgba The RGBA image buffer
  ///
  virtual int Mono16ToRgba(uint32_t width, uint32_t height, uint8_t *mono16, uint8_t *rgba) const = 0;

  ///
  /// \brief Convert Mono16 to RGBA on the CPU
  ///
  /// \param width The height of the image
  /// \param height The width of the image
  /// \param mono16 The monochrome 16 bit image
  /// \param rgb The RGB image buffer
  ///
  virtual int Mono16ToRgb(uint32_t width, uint32_t height, uint8_t *mono16, uint8_t *rgb) const = 0;

  ///
  /// \brief Convert YUV to RGB
  ///
  /// \param height The height of the image
  /// \param width The width of the image
  /// \param yuv The YUV:422 image buffer
  /// \param rgb The RGB image buffer
  ///
  virtual int YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) const = 0;

  ///
  /// \brief Scale the image to the target size for RGB
  ///
  /// \param source_height The height of the source image
  /// \param source_width The width of the source image
  /// \param source_rgb_buffer The source image buffer
  /// \param target_height The height of the target image
  /// \param target_width The width of the target image
  /// \param target_rgb_buffer The target image buffer
  ///
  virtual int ScaleToSizeRgb(uint32_t source_height, uint32_t source_width, uint8_t *source_rgb_buffer,
                             uint32_t target_height, uint32_t target_width, uint8_t *target_rgb_buffer) const = 0;

  ///
  /// \brief Scale the image to the target size for RGBA
  ///
  /// \param source_height The height of the source image
  /// \param source_width The width of the source image
  /// \param source_rgb_buffer The source image buffer
  /// \param target_height The height of the target image
  /// \param target_width The width of the target image
  /// \param target_rgb_buffer The target image buffer
  ///
  virtual int ScaleToSizeRgba(uint32_t source_height, uint32_t source_width, uint8_t *source_rgb_buffer,
                              uint32_t target_height, uint32_t target_width, uint8_t *target_rgb_buffer) const = 0;
};

}  // namespace mediax::video

#endif  // UTILS_COLOURSPACE_H_
