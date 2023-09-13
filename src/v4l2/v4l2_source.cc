//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief Session Announcement Protocol (SDP) implementation for listening to announcements of stream data. The SAP
/// packet contains the Session Description Protocol (SDP).
///
/// \file v4l2_source.cc
///

#include "v4l2/v4l2_source.h"

#include <glog/logging.h>
#include <sys/mman.h>

#include <cstring>
#include <string>

void PrintCaps(v4l2_capability* cap) {
  std::cout << "\tDriver: " << cap->driver << std::endl;
  std::cout << "\tCard: " << cap->card << std::endl;
  std::cout << "\tBus info: " << cap->bus_info << std::endl;
  std::cout << "\tVersion: " << (cap->version >> 16) << "." << ((cap->version >> 8) & 0xff) << "."
            << (cap->version & 0xff) << std::endl;
  std::cout << "\tCapabilities: " << std::hex << cap->capabilities << std::endl;
  std::cout << "\tDevice Caps: " << std::hex << cap->device_caps << std::endl;
}

V4L2Capture::V4L2Capture(std::string_view device, int width, int height)
    : device_(device), width_(width), height_(height) {}

int V4L2Capture::Initalise() {
  fd_ = open(std::string(device_).c_str(), O_RDWR);
  if (fd_ == -1) {
    LOG(ERROR) << "Failed to open device: " << device_;
    return -1;
  }

  struct v4l2_capability cap;
  if (ioctl(fd_, VIDIOC_QUERYCAP, &cap) == -1) {
    LOG(ERROR) << "Failed to query device capabilities";
    close(fd_);
    fd_ = -1;
    return -1;
  }
  PrintCaps(&cap);

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    LOG(ERROR) << "Device does not support video capture";
    close(fd_);
    fd_ = -1;
    return -1;
  }

  struct v4l2_format format = {0};
  format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  format.fmt.pix.width = width_;
  format.fmt.pix.height = height_;
  format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
  format.fmt.pix.field = V4L2_FIELD_NONE;

  if (ioctl(fd_, VIDIOC_S_FMT, &format) == -1) {
    LOG(ERROR) << "Failed to set pixel format for device using UYVY with width: " << width_
               << " and height: " << height_;
    close(fd_);
    fd_ = -1;
    return -1;
  }

  struct v4l2_requestbuffers request = {0};
  request.count = 1;
  request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  request.memory = V4L2_MEMORY_MMAP;

  if (ioctl(fd_, VIDIOC_REQBUFS, &request) == -1) {
    LOG(ERROR) << "Failed to request buffer";
    close(fd_);
    fd_ = -1;
    return -1;
  }

  struct v4l2_buffer buffer = {0};
  buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buffer.memory = V4L2_MEMORY_MMAP;
  buffer.index = 0;

  if (ioctl(fd_, VIDIOC_QUERYBUF, &buffer) == -1) {
    LOG(ERROR) << "Failed to query buffer";
    close(fd_);
    fd_ = -1;
    return -1;
  }

  buffer_ =
      static_cast<uint8_t*>(mmap(nullptr, buffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, buffer.m.offset));
  if (buffer_ == MAP_FAILED) {
    LOG(ERROR) << "Failed to map buffer";
    close(fd_);
    fd_ = -1;
    return -1;
  }

  if (ioctl(fd_, VIDIOC_STREAMON, &buffer.type) == -1) {
    LOG(ERROR) << "Failed to start capture";
    close(fd_);
    fd_ = -1;
    return -1;
  }

  struct v4l2_buffer buffer_info = {0};
  buffer_info.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buffer_info.memory = V4L2_MEMORY_MMAP;
  buffer_info.index = 0;
  if (ioctl(fd_, VIDIOC_QBUF, &buffer_info) == -1) {
    LOG(ERROR) << "Failed to queue buffer";
    return -1;
  }

  return 0;
}

V4L2Capture::~V4L2Capture() {
  if (fd_ != -1) {
    struct v4l2_buffer buffer = {0};
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory = V4L2_MEMORY_MMAP;
    buffer.index = 0;

    ioctl(fd_, VIDIOC_STREAMOFF, &buffer.type);
    if (buffer_ != nullptr) munmap(buffer_, width_ * height_ * 3);
    close(fd_);
  }
}

bool V4L2Capture::CaptureFrame(uint8_t* buffer) const {
  struct v4l2_buffer buffer_info = {0};
  buffer_info.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buffer_info.memory = V4L2_MEMORY_MMAP;

  if (ioctl(fd_, VIDIOC_DQBUF, &buffer_info) == -1) {
    LOG(ERROR) << "Failed to dequeue buffer";
    return false;
  }

  memcpy(buffer, buffer_, width_ * height_ * 2);

  if (ioctl(fd_, VIDIOC_QBUF, &buffer_info) == -1) {
    LOG(ERROR) << "Failed to queue buffer";
    return false;
  }

  return true;
}
