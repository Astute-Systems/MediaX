//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \file QtCommon.h
///

#ifndef QT6_QTCOMMON_H_
#define QT6_QTCOMMON_H_

#include <QByteArray>

#include "rtp/rtp.h"

/// A structure to store frames of video
struct Frame {
  /// The frame data
  QByteArray video;
  /// The frame width
  int width;
  /// The frame height
  int height;
  /// The frame encoding
  mediax::rtp::ColourspaceType encoding;
};

#endif  // QT6_QTCOMMON_H_
