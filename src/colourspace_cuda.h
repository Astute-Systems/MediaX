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
/// \file colourspace_cuda.h
///

#ifndef COLOURSPACE_CUDA_H
#define COLOURSPACE_CUDA_H

#include <stdint.h>

namespace video::cuda {

/// Helper functions for different colour space options
void RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv);
void RgbaToYuv(uint32_t width, uint32_t height, uint8_t *rgba, uint8_t *yuv);
void YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba);
void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb);

}  // namespace video::cuda

#endif  // COLOURSPACE_H