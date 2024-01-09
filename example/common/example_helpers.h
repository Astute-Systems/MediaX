//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief Some common functions used in examples
///
/// \file example_helpers.h
///

#include "rtp/rtp_types.h"

///
/// \brief Get the Mode as an enum
///
/// \param mode
/// \return mediax::rtp::ColourspaceType
///
mediax::rtp::ColourspaceType GetMode(int mode);

///
/// \brief Convert a mode to a string
///
/// \param mode
/// \return std::string
///
std::string ModeToString(int mode);
