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
#include <string_view>

namespace mediax::sap {

::mediax::rtp::StreamInformation SapToStreamInformation(const SdpMessage& sdp_message) {
  ::mediax::rtp::StreamInformation stream_information;
  stream_information.session_name = sdp_message.session_name;
  stream_information.hostname = sdp_message.ip_address;
  stream_information.port = sdp_message.port;
  stream_information.height = sdp_message.height;
  stream_information.width = sdp_message.width;
  stream_information.framerate = sdp_message.framerate;
  stream_information.encoding = SamplingToColourspaceType(sdp_message.sampling, sdp_message.bits_per_pixel);
  return stream_information;
}

mediax::rtp::ColourspaceType SamplingToColourspaceType(std::string_view sampling, uint32_t bits_per_pixel) {
  // Convert to lower case
  auto lower = std::string(sampling);
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

  if (sampling == "rgb") return mediax::rtp::ColourspaceType::kColourspaceRgb24;
  if (sampling == "YCbCr-4:2:2") return mediax::rtp::ColourspaceType::kColourspaceYuv;
  if ((sampling == "Mono") && (bits_per_pixel == 8)) return mediax::rtp::ColourspaceType::kColourspaceMono8;
  if ((sampling == "Mono") && (bits_per_pixel == 16)) return mediax::rtp::ColourspaceType::kColourspaceMono16;
  return mediax::rtp::ColourspaceType::kColourspaceUndefined;
}

std::string GetSdpColourspace(mediax::rtp::ColourspaceType colourspace) {
  switch (colourspace) {
    case mediax::rtp::ColourspaceType::kColourspaceRgb24:
      return "RGB";
    case mediax::rtp::ColourspaceType::kColourspaceYuv:
      return "YCbCr-4:2:2";
    case mediax::rtp::ColourspaceType::kColourspaceMono16:
      return "Mono";
    case mediax::rtp::ColourspaceType::kColourspaceMono8:
      return "Mono";
    default:
      return "unknown";
  }
}

}  // namespace mediax::sap
