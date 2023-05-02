
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

namespace video::cuda {

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

__global__ void YuvToRgbKernel(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x < width && y < height) {
    int index = y * width + x;
    int yIndex = index;
    int uIndex = (index / 4) + (height * width);
    int vIndex = (index / 4) + (height * width) + ((height * width) / 4);

    int Y = yuv[yIndex];
    int U = yuv[uIndex];
    int V = yuv[vIndex];

    int R = (int)(Y + 1.370705 * (V - 128));
    int G = (int)(Y - 0.698001 * (V - 128) - 0.337633 * (U - 128));
    int B = (int)(Y + 1.732446 * (U - 128));

    R = (R < 0) ? 0 : ((R > 255) ? 255 : R);
    G = (G < 0) ? 0 : ((G > 255) ? 255 : G);
    B = (B < 0) ? 0 : ((B > 255) ? 255 : B);

    int rgbIndex = index * 3;
    rgb[rgbIndex] = (uint8_t)R;
    rgb[rgbIndex + 1] = (uint8_t)G;
    rgb[rgbIndex + 2] = (uint8_t)B;
  }
}

void YuvToRgb(uint32_t height, uint32_t width, uint8_t *yuv, uint8_t *rgb) {
  uint8_t *d_yuv, *d_rgb;
  size_t size = height * width * 3 * sizeof(uint8_t);
  cudaMalloc((void **)&d_yuv, size);
  cudaMalloc((void **)&d_rgb, size);

  cudaMemcpy(d_yuv, yuv, size, cudaMemcpyHostToDevice);

  dim3 threadsPerBlock(16, 16);
  dim3 numBlocks((width + threadsPerBlock.x - 1) / threadsPerBlock.x,
                 (height + threadsPerBlock.y - 1) / threadsPerBlock.y);

  YuvToRgbKernel<<<numBlocks, threadsPerBlock>>>(height, width, d_yuv, d_rgb);

  cudaMemcpy(rgb, d_rgb, size, cudaMemcpyDeviceToHost);

  cudaFree(d_yuv);
  cudaFree(d_rgb);
}

__global__ void RgbToYuvKernel(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x < width && y < height) {
    int index = y * width + x;
    int rgbIndex = index * 3;
    int yIndex = index;
    int uIndex = (index / 4) + (height * width);
    int vIndex = (index / 4) + (height * width) + ((height * width) / 4);

    int R = rgb[rgbIndex];
    int G = rgb[rgbIndex + 1];
    int B = rgb[rgbIndex + 2];

    int Y = (int)(0.299 * R + 0.587 * G + 0.114 * B);
    int U = (int)((B - Y) * 0.565 + 128);
    int V = (int)((R - Y) * 0.713 + 128);

    Y = (Y < 0) ? 0 : ((Y > 255) ? 255 : Y);
    U = (U < 0) ? 0 : ((U > 255) ? 255 : U);
    V = (V < 0) ? 0 : ((V > 255) ? 255 : V);

    yuv[yIndex] = (uint8_t)Y;
    yuv[uIndex] = (uint8_t)U;
    yuv[vIndex] = (uint8_t)V;
  }
}

void RgbToYuv(uint32_t height, uint32_t width, uint8_t *rgb, uint8_t *yuv) {
  uint8_t *d_rgb, *d_yuv;
  size_t size = height * width * 3 * sizeof(uint8_t);
  cudaMalloc((void **)&d_rgb, size);
  cudaMalloc((void **)&d_yuv, size);

  cudaMemcpy(d_rgb, rgb, size, cudaMemcpyHostToDevice);

  dim3 threadsPerBlock(16, 16);
  dim3 numBlocks((width + threadsPerBlock.x - 1) / threadsPerBlock.x,
                 (height + threadsPerBlock.y - 1) / threadsPerBlock.y);

  RgbToYuvKernel<<<numBlocks, threadsPerBlock>>>(height, width, d_rgb, d_yuv);

  cudaMemcpy(yuv, d_yuv, size, cudaMemcpyDeviceToHost);

  cudaFree(d_rgb);
  cudaFree(d_yuv);
}

}  // namespace video::cuda
