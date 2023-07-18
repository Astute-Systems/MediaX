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

#ifndef COLOURSPACE_H
#define COLOURSPACE_H

#include <stdint.h>

namespace video {

/// Helper functions for different colour space options
class ColourSpace {
 public:
  ///
  /// \brief Construct a new Colour Space object
  ///
  ///
  ColourSpace();

  ///
  /// \brief Convert YUV to RGBA
  ///
  /// \param height The height of the image
  /// \param width The width of the image
  /// \param rgb The RGB image buffer
  /// \param yuv The YUV:422 image buffer
  ///
  void RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) const;

  ///
  /// \brief Convert RGBA to YUV
  ///
  /// \param width The width of the image
  /// \param height The height of the image
  /// \param rgba The RGB image buffer
  /// \param yuv The YUV:422 image buffer
  ///
  void RgbaToYuv(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *yuv) const;

  ///
  /// \brief Convert RGBA to RGB
  ///
  /// \param width
  /// \param height
  /// \param rgba
  /// \param rgb
  ///
  void RgbaToRgb(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *rgb) const;

  ///
  /// \brief Convert YUV to RGBA
  ///
  /// \param height The height of the image
  /// \param width The width of the image
  /// \param yuv The YUV:422 image buffer
  /// \param rgba The RGBA image buffer
  ///
  void YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba) const;

  ///
  /// \brief Convert YUV to RGB
  ///
  /// \param height The height of the image
  /// \param width The width of the image
  /// \param yuv The YUV:422 image buffer
  /// \param rgb The RGB image buffer
  ///
  void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) const;
};

}  // namespace video

#endif  // COLOURSPACE_H