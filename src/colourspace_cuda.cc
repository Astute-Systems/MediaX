
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
/// \brief Functions to convert between different colour spaces, these functions are optimized for Nvidia CUDA when
/// available.
///
/// \file colourspace_cuda.cc
///

#include <cuda_runtime.h>
#include <stdint.h>

#include <iostream>

// CUDA kernel to convert YUV to RGB
__global__ void YuvToRgbKernel(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x >= width || y >= height) return;

  int yuvIndex = y * width * 2 + x * 2;
  int rgbIndex = y * width * 3 + x * 3;

  int yVal = yuv[yuvIndex];
  int uVal = yuv[yuvIndex | 1] - 128;
  int vVal = yuv[yuvIndex & ~1] - 128;

  int c = yVal - 16;
  int d = uVal;
  int e = vVal;

  int r = (298 * c + 409 * e + 128) >> 8;
  int g = (298 * c - 100 * d - 208 * e + 128) >> 8;
  int b = (298 * c + 516 * d + 128) >> 8;

  rgb[rgbIndex] = min(max(r, 0), 255);
  rgb[rgbIndex + 1] = min(max(g, 0), 255);
  rgb[rgbIndex + 2] = min(max(b, 0), 255);
}

void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) {
  if (!rgb || !yuv) {
    return;
  }

  uint8_t *dev_yuv, *dev_rgb;
  size_t yuv_size = height * width * 2 * sizeof(uint8_t);
  size_t rgb_size = height * width * 3 * sizeof(uint8_t);

  // Allocate memory on GPU
  cudaMalloc((void **)&dev_yuv, yuv_size);
  cudaMalloc((void **)&dev_rgb, rgb_size);

  // Copy input data to GPU
  cudaMemcpy(dev_yuv, yuv, yuv_size, cudaMemcpyHostToDevice);

  // Launch CUDA kernel
  dim3 blockDim(16, 16);
  dim3 gridDim((width + blockDim.x - 1) / blockDim.x, (height + blockDim.y - 1) / blockDim.y);
  YuvToRgbKernel<<<gridDim, blockDim>>>(height, width, dev_yuv, dev_rgb);

  // Copy result back to host
  cudaMemcpy(rgb, dev_rgb, rgb_size, cudaMemcpyDeviceToHost);

  // Clean up
  cudaFree(dev_yuv);
  cudaFree(dev_rgb);
}