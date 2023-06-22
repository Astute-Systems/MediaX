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
/// \file v4l2_source.h
///

#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <iostream>

/// Class for Video 4 Linux 2 (V4L2) capture
class V4L2Capture {
 public:
  ///
  /// \brief Construct a new V4L2Capture object
  ///
  /// \param device The device to capture from
  /// \param width The width of the frame
  /// \param height The height of the frame
  ///
  V4L2Capture(const char* device, int width, int height);

  ///
  /// \brief Destroy the V4L2Capture::V4L2Capture object
  ///
  ///
  ~V4L2Capture();

  ///
  /// \brief Capture a frame from the V4L2 device
  ///
  /// \param buffer The buffer to store the frame in
  /// \return true
  /// \return false
  ///
  bool CaptureFrame(uint8_t* buffer) const;

 private:
  /// \brief The file descriptor for the V4L2 device
  int fd_ = -1;
  /// \brief The width of the frame
  int width_;
  /// \brief The height of the frame
  int height_;
  /// \brief The buffer to store the frame in
  uint8_t* buffer_ = nullptr;
};
