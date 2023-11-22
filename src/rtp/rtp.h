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

#if GST_SUPPORTED
#include "h264/gst/nvidia/rtp_h264_depayloader.h"
#include "h264/gst/nvidia/rtp_h264_payloader.h"
#include "h264/gst/vaapi/rtp_h264_depayloader.h"
#include "h264/gst/vaapi/rtp_h264_payloader.h"
#include "h265/gst/vaapi/rtp_h265_depayloader.h"
#include "h265/gst/vaapi/rtp_h265_payloader.h"
#endif
#include "rtp/rtp_depayloader.h"
#include "rtp/rtp_payloader.h"
#include "rtp/rtp_utils.h"
#include "uncompressed/rtp_uncompressed_depayloader.h"
#include "uncompressed/rtp_uncompressed_payloader.h"
#include "wrappers/rtp_sap_wrapper.h"

#if CUDA_ENABLED
#include "utils/colourspace_cuda.h"
#else
#include "utils/colourspace_cpu.h"
#endif

/// The H.264 video compression namespace
namespace mediax::rtp::h264 {
// Dummy to get doxygen comment in
}

#endif  // RTP_RTP_H_
