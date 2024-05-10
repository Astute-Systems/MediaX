//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the MediaX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief A simple video receiver example
///
/// \file utils.h
///

#ifndef UTILS_H
#define UTILS_H
#include <assert.h>
#include <capture.h>
#include <capture_yuyv.h>
#include <errno.h>
#include <fcntl.h>   // low-level i/o
#include <getopt.h>  // getopt_long()
#include <linux/videodev2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

struct image_info_t {
  int stride;
  int width;
  int height;
};

void YUV422toBGR888(int width, int height, unsigned char *src, unsigned char *dst);
int SaveImageUncompressed(const unsigned char *image, const char *szFilename, image_info_t *info, int type);

#endif  // UTILS_H