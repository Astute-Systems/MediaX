//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video types
///
/// \file rtp.h

#ifndef RTP_RTP_H_
#define RTP_RTP_H_

#include "h264/gst/rtph264_depayloader.h"
#include "h264/gst/rtph264_payloader.h"
#include "raw/rtpvraw_depayloader.h"
#include "raw/rtpvraw_payloader.h"
#include "rtp/rtp_depayloader.h"
#include "rtp/rtp_payloader.h"
#include "rtp/rtp_utils.h"

#if CUDA_ENABLED
#include "utils/colourspace_cuda.h"
#else
#include "utils/colourspace_cpu.h"
#endif

#endif  // RTP_RTP_H_
