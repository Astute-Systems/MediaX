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
extern "C" {
#include "libswscale/swscale.h"
}
void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba) {
  SwsContext *ctx =
      sws_getContext(width, height, AV_PIX_FMT_YUYV422, width, height, AV_PIX_FMT_RGB24, SWS_BICUBIC, 0, 0, 0);
  uint8_t *inData[1] = {(uint8_t *)yuv};    // RGB24 have one plane
  uint8_t *outData[1] = {(uint8_t *)rgba};  // YUYV have one plane
  int inLinesize[1] = {(int)(width * 2)};   // YUYV stride
  int outLinesize[1] = {(int)(width * 3)};  // RGB srtide
  sws_scale(ctx, inData, inLinesize, 0, height, outData, outLinesize);
}

void YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) {
  SwsContext *ctx =
      sws_getContext(width, height, AV_PIX_FMT_UYVY422, width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, 0, 0, 0);
  uint8_t *inData[1] = {(uint8_t *)yuv};    // YUV have one plane
  uint8_t *outData[1] = {(uint8_t *)rgb};   // RGBA have one plane
  int inLinesize[1] = {(int)(width * 2)};   // YUYV stride
  int outLinesize[1] = {(int)(width * 4)};  // RGBA srtide
  sws_scale(ctx, inData, inLinesize, 0, height, outData, outLinesize);
}

void RgbaToYuv(uint32_t height, uint32_t width, uint8_t *rgba, uint8_t *yuv) {
  SwsContext *ctx = sws_getContext(width, height, AV_PIX_FMT_RGBA, width, height, AV_PIX_FMT_YUYV422, 0, 0, 0, 0);
  uint8_t *inData[1] = {(uint8_t *)rgba};   // RGB24 have one plane
  uint8_t *outData[1] = {(uint8_t *)yuv};   // YUYV have one plane
  int inLinesize[1] = {(int)(width * 4)};   // RGB stride
  int outLinesize[1] = {(int)(width * 2)};  // YUYV stride
  sws_scale(ctx, inData, inLinesize, 0, height, outData, outLinesize);
}

void RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) {
  SwsContext *ctx =
      sws_getContext(width, height, AV_PIX_FMT_RGB24, width, height, AV_PIX_FMT_YUYV422, SWS_BICUBIC, 0, 0, 0);
  uint8_t *inData[1] = {(uint8_t *)rgb};    // RGB24 have one plane
  uint8_t *outData[1] = {(uint8_t *)yuv};   // YUYV have one plane
  int inLinesize[1] = {(int)(width * 3)};   // RGB stride
  int outLinesize[1] = {(int)(width * 2)};  // YUYV stride
  sws_scale(ctx, inData, inLinesize, 0, height, outData, outLinesize);
}