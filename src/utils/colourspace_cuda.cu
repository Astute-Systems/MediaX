//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief Functions to convert between different colour spaces, these functions are optimized for Nvidia CUDA when
/// available.
///
/// \file colourspace_cuda.cc
///

#include <cuda_runtime.h>
#include <stdint.h>

#include <iostream>

#include "utils/colourspace_cpu.h"

namespace video {

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

void ColourSpaceCuda::YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) const {
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

__global__ void yuvToRgbaKernel(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  if (x < width && y < height) {
    int yIndex = y * width + x;
    int uIndex = (y / 2) * (width / 2) + (x / 2) + (height * width);
    int vIndex = (y / 2) * (width / 2) + (x / 2) + (height * width) + ((height * width) / 4);
    int r, g, b;
    int yValue = yuv[yIndex];
    int uValue = yuv[uIndex];
    int vValue = yuv[vIndex];
    uValue -= 128;
    vValue -= 128;
    r = (int)(yValue + 1.13983f * vValue);
    g = (int)(yValue - 0.39465f * uValue - 0.58060f * vValue);
    b = (int)(yValue + 2.03211f * uValue);
    r = min(max(0, r), 255);
    g = min(max(0, g), 255);
    b = min(max(0, b), 255);
    int rgbaIndex = yIndex * 4;
    rgba[rgbaIndex] = (uint8_t)r;
    rgba[rgbaIndex + 1] = (uint8_t)g;
    rgba[rgbaIndex + 2] = (uint8_t)b;
    rgba[rgbaIndex + 3] = 255;
  }
}

void ColourSpaceCuda::YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba) const {
  int block_size = 32;
  dim3 dimBlock(block_size, block_size);
  dim3 dimGrid((width + dimBlock.x - 1) / dimBlock.x, (height + dimBlock.y - 1) / dimBlock.y);
  yuvToRgbaKernel<<<dimGrid, dimBlock>>>(height, width, yuv, rgba);
  cudaDeviceSynchronize();
}

__global__ void rgbToYuvKernel(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  if (x < width && y < height) {
    int index = y * width + x;
    int r = rgb[index * 3];
    int g = rgb[index * 3 + 1];
    int b = rgb[index * 3 + 2];
    int yValue = (int)(0.299f * r + 0.587f * g + 0.114f * b);
    int uValue = (int)(-0.14713f * r - 0.28886f * g + 0.436f * b);
    int vValue = (int)(0.615f * r - 0.51499f * g - 0.10001f * b);
    yValue = min(max(0, yValue), 255);
    uValue = min(max(0, uValue + 128), 255);
    vValue = min(max(0, vValue + 128), 255);
    int yIndex = y * width + x;
    int uIndex = (y / 2) * (width / 2) + (x / 2) + (height * width);
    int vIndex = (y / 2) * (width / 2) + (x / 2) + (height * width) + ((height * width) / 4);
    yuv[yIndex] = (uint8_t)yValue;
    yuv[uIndex] = (uint8_t)uValue;
    yuv[vIndex] = (uint8_t)vValue;
  }
}

void RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) const {
  int block_size = 32;
  dim3 dimBlock(block_size, block_size);
  dim3 dimGrid((width + dimBlock.x - 1) / dimBlock.x, (height + dimBlock.y - 1) / dimBlock.y);
  rgbToYuvKernel<<<dimGrid, dimBlock>>>(height, width, rgb, yuv);
  cudaDeviceSynchronize();
}

__global__ void rgbaToYuvKernel(uint32_t height, uint32_t width, uint8_t *rgba, uint8_t *yuv) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  if (x < width && y < height) {
    int index = y * width + x;
    int r = rgba[index * 4];
    int g = rgba[index * 4 + 1];
    int b = rgba[index * 4 + 2];
    int yValue = (int)(0.299f * r + 0.587f * g + 0.114f * b);
    int uValue = (int)(-0.14713f * r - 0.28886f * g + 0.436f * b);
    int vValue = (int)(0.615f * r - 0.51499f * g - 0.10001f * b);
    yValue = min(max(0, yValue), 255);
    uValue = min(max(0, uValue + 128), 255);
    vValue = min(max(0, vValue + 128), 255);
    int yIndex = y * width + x;
    int uIndex = (y / 2) * (width / 2) + (x / 2) + (height * width);
    int vIndex = (y / 2) * (width / 2) + (x / 2) + (height * width) + ((height * width) / 4);
    yuv[yIndex] = (uint8_t)yValue;
    yuv[uIndex] = (uint8_t)uValue;
    yuv[vIndex] = (uint8_t)vValue;
  }
}

void ColourSpaceCuda::RgbaToYuv(uint32_t height, uint32_t width, uint8_t *rgba, uint8_t *yuv) {
  int block_size = 32;
  dim3 dimBlock(block_size, block_size);
  dim3 dimGrid((width + dimBlock.x - 1) / dimBlock.x, (height + dimBlock.y - 1) / dimBlock.y);
  rgbaToYuvKernel<<<dimGrid, dimBlock>>>(height, width, rgba, yuv);
  cudaDeviceSynchronize();
}

}  // namespace video
