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

namespace video {

/// Helper functions for different colour space options
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
  ~ColourSpace() = default;

  ///
  /// \brief Convert YUV to RGBA
  ///
  /// \param height The height of the image
  /// \param width The width of the image
  /// \param rgb The RGB image buffer
  /// \param yuv The YUV:422 image buffer
  ///
  virtual void RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) const = 0;

  ///
  /// \brief Convert RGB to Monochrome 8 bits ber pixel
  ///
  /// \param height The height of the image
  /// \param width The width of the image
  /// \param rgb The RGB image buffer
  /// \param mono8 The monochrome 8 image buffer
  ///
  virtual void RgbToMono8(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *mono8) const = 0;

  ///
  /// \brief Convert RGB to Monochrome 16 bits ber pixel
  ///
  /// \param height  The height of the image
  /// \param width The width of the image
  /// \param rgb  The RGB image buffer
  /// \param mono16 The monochrome 16 image buffer
  ///
  virtual void RgbToMono16(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *mono16) const = 0;

  ///
  /// \brief Convert RGBA to YUV
  ///
  /// \param width The width of the image
  /// \param height The height of the image
  /// \param rgba The RGB image buffer
  /// \param yuv The YUV:422 image buffer
  ///
  virtual void RgbaToYuv(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *yuv) const = 0;

  ///
  /// \brief Convert RGBA to RGB
  ///
  /// \param width The height of the image
  /// \param height The width of the image
  /// \param rgba The RGBA image buffer
  /// \param rgb The RGB image buffer
  ///
  virtual void RgbaToRgb(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *rgb) const = 0;

  ///
  /// \brief Convert RGB to RGBA
  ///
  /// \param width The height of the image
  /// \param height The width of the image
  /// \param rgba The RGBA image buffer
  /// \param rgb The RGB image buffer
  ///
  virtual void RgbToRgba(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *rgb) const = 0;

  ///
  /// \brief Convert YUV to RGBA
  ///
  /// \param height The height of the image
  /// \param width The width of the image
  /// \param yuv The YUV:422 image buffer
  /// \param rgba The RGBA image buffer
  ///
  virtual void YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba) const = 0;

  ///
  /// \brief Convert YUV to RGB
  ///
  /// \param height The height of the image
  /// \param width The width of the image
  /// \param yuv The YUV:422 image buffer
  /// \param rgb The RGB image buffer
  ///
  virtual void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) const = 0;
};

}  // namespace video

#endif  // UTILS_COLOURSPACE_H_
