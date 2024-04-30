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
/// \file capture_yuyv.h
///

#ifndef CAPTURE_YUYV_H
#define CAPTURE_YUYV_H

typedef struct {
  int stride;
  int width;
  int height;
} image_info_t;

int save_image_uncompressed(const unsigned char *image, const char *szFilename, image_info_t *info, int type);

class V4l2_Capture_YUYV {
 public:
  void init_mmap(void);
  void init_userp(unsigned int buffer_size);
  void init_device(void);
  void uninit_device(void);
  void mainloop(void);
  void close_device(void);
  void open_device(void);
  void start_capturing(void);
  void stop_capturing(void);
};

#endif  // CAPTURE_YUYV_H