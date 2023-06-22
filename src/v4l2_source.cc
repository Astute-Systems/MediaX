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

#include "v4l2_source.h"

#include <sys/mman.h>

#include <cstring>

V4L2Capture::V4L2Capture(const char* device, int width, int height) : width_(width), height_(height) {
  fd_ = open(device, O_RDWR);
  if (fd_ == -1) {
    std::cerr << "Failed to open device: " << device << std::endl;
    return;
  }

  struct v4l2_capability cap;
  if (ioctl(fd_, VIDIOC_QUERYCAP, &cap) == -1) {
    std::cerr << "Failed to query device capabilities" << std::endl;
    close(fd_);
    fd_ = -1;
    return;
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    std::cerr << "Device does not support video capture" << std::endl;
    close(fd_);
    fd_ = -1;
    return;
  }

  struct v4l2_format fmt = {0};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = width_;
  fmt.fmt.pix.height = height_;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;

  if (ioctl(fd_, VIDIOC_S_FMT, &fmt) == -1) {
    std::cerr << "Failed to set pixel format" << std::endl;
    close(fd_);
    fd_ = -1;
    return;
  }

  struct v4l2_requestbuffers req = {0};
  req.count = 1;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (ioctl(fd_, VIDIOC_REQBUFS, &req) == -1) {
    std::cerr << "Failed to request buffer" << std::endl;
    close(fd_);
    fd_ = -1;
    return;
  }

  struct v4l2_buffer buf = {0};
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = 0;

  if (ioctl(fd_, VIDIOC_QUERYBUF, &buf) == -1) {
    std::cerr << "Failed to query buffer" << std::endl;
    close(fd_);
    fd_ = -1;
    return;
  }

  buffer_ = static_cast<uint8_t*>(mmap(nullptr, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, buf.m.offset));
  if (buffer_ == MAP_FAILED) {
    std::cerr << "Failed to map buffer" << std::endl;
    close(fd_);
    fd_ = -1;
    return;
  }

  if (ioctl(fd_, VIDIOC_STREAMON, &buf.type) == -1) {
    std::cerr << "Failed to start capture" << std::endl;
    close(fd_);
    fd_ = -1;
    return;
  }
}

V4L2Capture::~V4L2Capture() {
  if (fd_ != -1) {
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    ioctl(fd_, VIDIOC_STREAMOFF, &buf.type);
    munmap(buffer_, width_ * height_ * 3);
    close(fd_);
  }
}

bool V4L2Capture::CaptureFrame(uint8_t* buffer) const {
  struct v4l2_buffer buf = {0};
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  if (ioctl(fd_, VIDIOC_QBUF, &buf) == -1) {
    std::cerr << "Failed to queue buffer" << std::endl;
    return false;
  }

  if (ioctl(fd_, VIDIOC_DQBUF, &buf) == -1) {
    std::cerr << "Failed to dequeue buffer" << std::endl;
    return false;
  }

  memcpy(buffer, buffer_, width_ * height_ * 3);

  return true;
}
