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
/// \file sap_utils.cc
///

#include "sap_utils.h"

std::string GetSdpColourspace(ColourspaceType colourspace) {
  switch (colourspace) {
    case ColourspaceType::kColourspaceRgb24:
      return "RGB";
    case ColourspaceType::kColourspaceYuv:
      return "YCbCr-4:2:2";
    case ColourspaceType::kColourspaceMono16:
      return "Mono";
    case ColourspaceType::kColourspaceMono8:
      return "Mono";
    default:
      return "unknown";
  }
}
