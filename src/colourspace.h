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