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
/// \brief Functions to convert between different colour spaces
///
/// \file colourspace.h
///

#ifndef COLOURSPACE_H
#define COLOURSPACE_H

#include <stdint.h>

namespace video {

/// Helper functions for different colour space options

///
/// \brief Convert YUV to RGB
///
/// \param height The height of the image
/// \param width The width of the image
/// \param yuv The YUV:422 image buffer
/// \param rgba  The RGBA image buffer
///
void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba);

///
/// \brief Convert YUV to RGBA
///
/// \param height The height of the image
/// \param width The width of the image
/// \param rgb The RGB image buffer
/// \param yuv The YUV:422 image buffer
///
void RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv);

///
/// \brief Convert RGBA to YUV
///
/// \param width The width of the image
/// \param height The height of the image
/// \param rgba The RGB image buffer
/// \param yuv The YUV:422 image buffer
///
void RgbaToYuv(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *yuv);

///
/// \brief Convert YUV to RGBA
///
/// \param height The height of the image
/// \param width The width of the image
/// \param yuv The YUV:422 image buffer
/// \param rgba The RGBA image buffer
///
void YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba);

///
/// \brief Convert YUV to RGB
///
/// \param height The height of the image
/// \param width The width of the image
/// \param *yuv The YUV:422 image buffer
/// \param *rgb The RGB image buffer
///
void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb);

}  // namespace video

#endif  // COLOURSPACE_H