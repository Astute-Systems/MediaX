
//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video class for H.264 DEF-STAN 00-82 video streams
///
/// \file rtph264_depayloader.cc
///

#include "h264/vaapi/rtph264_depayloader.h"

#include <va/va.h>
#include <va/va_drm.h>
#include <va/va_x11.h>

#include "rtp/rtp_types.h"

namespace mediax::rtp::h264::vaapi {

///
/// \brief Decode a frame
///
/// \param va_display
/// \param va_context
/// \param va_surface
/// \param frame_data
/// \param frame_size
/// \return StatusCode
///
StatusCode DecodeFrame(VADisplay va_display, VAContextID va_context, VASurfaceID va_surface, const uint8_t *frame_data,
                       int frame_size) {
  return StatusCode::kStatusOk;
}

}  // namespace mediax::rtp::h264::vaapi
