//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video class base class for all RTP depayloaders
///
/// \file QtRtpH264Depayloader.cc
///

#include "qt/QtRtpDepayloader.h"

namespace mediax::qt {

QtRtpDepayloader::QtRtpDepayloader(QObject *parent) : QObject(parent) {}

}  // namespace mediax::qt
