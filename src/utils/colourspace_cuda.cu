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

#include "utils/colourspace_cuda.h"

namespace mediax::video {

ColourSpaceCuda::ColourSpaceCuda() {
  // Initialise CUDA
  cudaError_t cudaStatus = cudaSetDevice(0);
  if (cudaStatus != cudaSuccess) {
    fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
  }
}

__global__ void RgbToYuvKernel<<<dimGrid, dimBlock>>>(height, width, rgb, yuv) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x >= width || y >= height) return;

  int rgbIndex = y * width * 3 + x * 3;
  int yuvIndex = y * width * 2 + x * 2;

  int r = rgb[rgbIndex];
  int g = rgb[rgbIndex + 1];
  int b = rgb[rgbIndex + 2];

  int yVal = (int)(0.299f * r + 0.587f * g + 0.114f * b);
  int uVal = (int)(-0.14713f * r - 0.28886f * g + 0.436f * b);
  int vVal = (int)(0.615f * r - 0.51499f * g - 0.10001f * b);

  yVal = min(max(0, yVal), 255);
  uVal = min(max(0, uVal + 128), 255);
  vVal = min(max(0, vVal + 128), 255);

  yuv[yuvIndex] = (uint8_t)yVal;
  yuv[yuvIndex | 1] = (uint8_t)uVal;
  yuv[yuvIndex & ~1] = (uint8_t)vVal;
}

int RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) const {
  int block_size = 32;
  dim3 dimBlock(block_size, block_size);
  dim3 dimGrid((width + dimBlock.x - 1) / dimBlock.x, (height + dimBlock.y - 1) / dimBlock.y);
  RgbToYuvKernel<<<dimGrid, dimBlock>>>(height, width, rgb, yuv);
  cudaDeviceSynchronize();
}

__global__ void RgbToMono8Kernel<<<gridDim, blockDim>>>(height, width, dev_rgb, dev_mono8) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x >= width || y >= height) return;

  int rgbIndex = y * width * 3 + x * 3;
  int mono8Index = y * width + x;

  int r = dev_rgb[rgbIndex];
  int g = dev_rgb[rgbIndex + 1];
  int b = dev_rgb[rgbIndex + 2];

  int mono8Val = (int)(0.299f * r + 0.587f * g + 0.114f * b);

  dev_mono8[mono8Index] = (uint8_t)mono8Val;
}

int RgbToMono8(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *mono8) const {
  if (!rgb || !mono8) {
    return;
  }

  uint8_t *dev_rgb, *dev_mono8;
  size_t rgb_size = height * width * 3 * sizeof(uint8_t);
  size_t mono8_size = height * width * sizeof(uint8_t);

  // Allocate memory on GPU
  cudaMalloc((void **)&dev_rgb, rgb_size);
  cudaMalloc((void **)&dev_mono8, mono8_size);

  // Copy input data to GPU
  cudaMemcpy(dev_rgb, rgb, rgb_size, cudaMemcpyHostToDevice);

  // Launch CUDA kernel
  dim3 blockDim(16, 16);
  dim3 gridDim((width + blockDim.x - 1) / blockDim.x, (height + blockDim.y - 1) / blockDim.y);
  RgbToMono8Kernel<<<gridDim, blockDim>>>(height, width, dev_rgb, dev_mono8);

  // Copy result back to host
  cudaMemcpy(mono8, dev_mono8, mono8_size, cudaMemcpyDeviceToHost);

  // Clean up
  cudaFree(dev_rgb);
  cudaFree(dev_mono8);
}

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

int ColourSpaceCuda::YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) const {
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

int ColourSpaceCuda::YuvToRgba(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgba) const {
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

int RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) const {
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

int ColourSpaceCuda::RgbaToYuv(uint32_t height, uint32_t width, uint8_t *rgba, uint8_t *yuv) {
  int block_size = 32;
  dim3 dimBlock(block_size, block_size);
  dim3 dimGrid((width + dimBlock.x - 1) / dimBlock.x, (height + dimBlock.y - 1) / dimBlock.y);
  rgbaToYuvKernel<<<dimGrid, dimBlock>>>(height, width, rgba, yuv);
  cudaDeviceSynchronize();
  return 0;
}

}  // namespace mediax::video
