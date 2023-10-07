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

#include "sap/sap_utils.h"
// for std::transform
#include <algorithm>

namespace mediax::sap {

ColourspaceType SamplingToColourspaceType(std::string sampling, uint32_t bits_per_pixel) {
  // Convert to lower case
  std::string lower = sampling;
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

  if (sampling == "rgb") return ColourspaceType::kColourspaceRgb24;
  if (sampling == "YCbCr-4:2:2") return ColourspaceType::kColourspaceYuv;
  if ((sampling == "Mono") && (bits_per_pixel == 8)) return ColourspaceType::kColourspaceMono8;
  if ((sampling == "Mono") && (bits_per_pixel == 16)) return ColourspaceType::kColourspaceMono16;
  return ColourspaceType::kColourspaceUndefined;
}

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

}  // namespace mediax::sap
