//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief Session Announcement Protocol (SDP) implementation for listening to announcements of stream data. The SAP
/// packet contains the Session Description Protocol (SDP).
///
/// \file sap_utils.h
///

#ifndef SAP_SAP_UTILS_H_
#define SAP_SAP_UTILS_H_

#include <string>

#include "rtp/rtp_types.h"
#include "sap/sap_listener.h"

/// The Session Announcment Protocol / Session Description Protocol (SAP/SDP) namespace
namespace mediax::sap {

///
/// \brief Convert the SDP message to a StreamInformation object
///
/// \param sdp_message
/// \return ::mediax::rtp::StreamInformation
///
::mediax::rtp::StreamInformation SapToStreamInformation(const SdpMessage& sdp_message);

///
/// \brief Convert a string to a ColourspaceType
///
/// \param sampling The sampling string
/// \param bits_per_pixel The number of bits per pixel
/// \return ColourspaceType
///
mediax::rtp::ColourspaceType SamplingToColourspaceType(std::string_view sampling, uint32_t bits_per_pixel);

///
/// \brief Get the Sdp Colourspace object
///
/// \param colourspace The colourspace to get the SDP string for
/// \return std::string
///
std::string GetSdpColourspace(mediax::rtp::ColourspaceType colourspace);

#endif  // SAP_SAP_UTILS_H_

}  // namespace mediax::sap
