
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

#include "h264/native/rtph264_depayloader.h"

#include <glog/logging.h>
#include <va/va.h>
#include <va/va_drm.h>
#include <va/va_x11.h>

#include "rtp/rtp_types.h"

namespace mediax::rtp::h264 {

StatusCode DecodeFrame(VADisplay va_display, VAContextID va_context, VASurfaceID va_surface, const uint8_t *frame_data,
                       int frame_size) {
  VAConfigID va_config;
  VAStatus va_status;
  VABufferID va_buffer;
  VAImage va_image;
  void *va_image_data;

  va_status = vaCreateConfig(va_display, VAProfileH264High, VAEntrypointVLD, nullptr, 0, &va_config);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to create VA config";
    return StatusCode::kStatusError;
  }

  auto type = (VABufferType)0;
  va_status = vaCreateBuffer(va_display, va_context, type, 0, 0, nullptr, &va_buffer);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to create VA buffer";
    return StatusCode::kStatusError;
  }

  va_status = vaCreateSurfaces(va_display, VA_RT_FORMAT_YUV420, 640, 480, &va_surface, 1, nullptr, 0);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to create VA surface";
    return StatusCode::kStatusError;
  }

  va_status = vaCreateContext(va_display, va_config, 0, 0, 0, &va_surface, 1, &va_context);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to create VA context";
    return StatusCode::kStatusError;
  }

  va_status = vaBeginPicture(va_display, va_context, va_surface);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to begin VA picture";
    return StatusCode::kStatusError;
  }

  va_status = vaCreateBuffer(va_display, va_context, VASliceDataBufferType, frame_size, 1,
                             const_cast<uint8_t *>(frame_data), &va_buffer);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to create VA buffer";
    return StatusCode::kStatusError;
  }

  va_status = vaRenderPicture(va_display, va_context, &va_buffer, 1);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to render VA picture";
    return StatusCode::kStatusError;
  }

  va_status = vaEndPicture(va_display, va_context);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to end VA picture";
    return StatusCode::kStatusError;
  }

  va_status = vaSyncSurface(va_display, va_surface);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to sync VA surface";
    return StatusCode::kStatusError;
  }

  va_status = vaDeriveImage(va_display, va_surface, &va_image);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to derive VA image";
    return StatusCode::kStatusError;
  }

  va_status = vaMapBuffer(va_display, va_image.buf, &va_image_data);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to map VA buffer";
    return StatusCode::kStatusError;
  }

  // Do something with the decoded image data

  va_status = vaUnmapBuffer(va_display, va_image.buf);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to unmap VA buffer";
    return StatusCode::kStatusError;
  }

  va_status = vaDestroyImage(va_display, va_image.image_id);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to destroy VA image";
    return StatusCode::kStatusError;
  }

  va_status = vaDestroyBuffer(va_display, va_buffer);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to destroy VA buffer";
    return StatusCode::kStatusError;
  }

  va_status = vaDestroyContext(va_display, va_context);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to destroy VA context";
    return StatusCode::kStatusError;
  }

  va_status = vaDestroySurfaces(va_display, &va_surface, 1);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to destroy VA surface";
    return StatusCode::kStatusError;
  }

  va_status = vaDestroyConfig(va_display, va_config);
  if (va_status != VA_STATUS_SUCCESS) {
    LOG(ERROR) << "Failed to destroy VA config";
    return StatusCode::kStatusError;
  }

  return StatusCode::kStatusOk;
}

}  // namespace mediax::rtp::h264
