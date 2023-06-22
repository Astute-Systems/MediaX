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
/// \file sap_utils.h
///

#ifndef __SAP_UTILS_H__
#define __SAP_UTILS_H__

#include <string>

#include "rtp_types.h"

///
/// \brief Get the Sdp Colourspace object
///
/// \param colourspace The colourspace to get the SDP string for
/// \return std::string
///
std::string GetSdpColourspace(ColourspaceType colourspace);

#endif  // __SAP_UTILS_H__
