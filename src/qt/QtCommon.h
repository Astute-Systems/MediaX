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

#ifndef QT_QTCOMMON_H_
#define QT_QTCOMMON_H_

#include <QByteArray>

#include "rtp/rtp.h"

struct Frame {
  QByteArray video;
  int width;
  int height;
  mediax::rtp::ColourspaceType encoding;
};

#endif  // QT_QTCOMMON_H_