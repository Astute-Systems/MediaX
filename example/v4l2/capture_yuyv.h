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
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/ioctl.h>

#include <string>

class V4l2_Capture_YUYV {
 public:
  V4l2_Capture_YUYV(std::string dev_name, IoMethod io = IoMethod::IO_METHOD_MMAP);
  void InitaliseMmap(void);
  void InitaliseUserp(unsigned int buffer_size);
  void InitaliseDevice(void);
  void UninitaliseDevice(void);
  void Run(void);
  int read_frame(int count);
  void CloseDevice(void);
  void OpenDevice(void);
  void StartDevice(void);
  void StopDevice(void);

 private:
  std::string dev_name_;
  IoMethod io_;
};

#endif  // CAPTURE_YUYV_H