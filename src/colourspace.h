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
/// \file colourspace.h
///

#ifndef COLOURSPACE_H
#define COLOURSPACE_H

#include <stdint.h>

/// Helper functions for different colour space options
void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba);
void RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv);
void RgbaToYuv(uint32_t width, uint32_t height, uint8_t *rgb_buffer, uint8_t *yuv_buffer);
void YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba);
void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb);

#endif  // COLOURSPACE_H