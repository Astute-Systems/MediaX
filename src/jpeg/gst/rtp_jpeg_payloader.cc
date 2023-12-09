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
/// \file rtp_jpeg_payloader.cc
///

#include "jpeg/gst/rtp_jpeg_payloader.h"

#include <glog/logging.h>
#include <gst/app/gstappsrc.h>
#include <gst/gst.h>

#include "jpeg/gst/rtp_jpeg_depayloader.h"

namespace mediax::rtp::jpeg::gst {

RtpJpegGstPayloader::RtpJpegGstPayloader() = default;

RtpJpegGstPayloader::~RtpJpegGstPayloader() = default;

void RtpJpegGstPayloader::SetStreamInfo(const ::mediax::rtp::StreamInformation &stream_information) {
  GetEgressPort().encoding = stream_information.encoding;
  GetEgressPort().height = stream_information.height;
  GetEgressPort().width = stream_information.width;
  GetEgressPort().framerate = stream_information.framerate;
  GetEgressPort().name = stream_information.session_name;
  GetEgressPort().hostname = stream_information.hostname;
  GetEgressPort().port_no = stream_information.port;
  GetEgressPort().settings_valid = true;
}

int RtpJpegGstPayloader::Transmit(unsigned char *new_buffer, bool timeout) {
  if (!started_) {
    DLOG(ERROR) << "RTP JPEG payloader not started";
    return -1;
  }

  // Gstreamer send appsrc
  // Send a frame
  GstElement *appsrc = gst_bin_get_by_name(GST_BIN(pipeline_), "rtp-jpeg-appsrc");
  int size = GetEgressPort().width * GetEgressPort().height * 3;
  GstBuffer *buffer = gst_buffer_new_allocate(nullptr, size, nullptr);

  // Define info
  GstMapInfo info;
  gst_buffer_map(buffer, &info, GST_MAP_WRITE);

  // Copy some data
  memcpy(info.data, new_buffer, size);
  gst_buffer_unmap(buffer, &info);

  // Set the buffer timestamp
  GST_BUFFER_PTS(buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_DTS(buffer) = GST_CLOCK_TIME_NONE;

  // Push the buffer to the appsrc and emit signal
  gst_app_src_push_buffer(GST_APP_SRC(appsrc), buffer);

  // Release the appsrc
  gst_object_unref(appsrc);

  return 0;
}

bool RtpJpegGstPayloader::Open() {
  // Setup a gstreamer pipeline to decode H.264 with Intel

  // Create a pipeline
  pipeline_ = gst_pipeline_new("rtp-jpeg-pipeline");
  // Create a custom appsrc element to receive the H.264 stream
  GstElement *appsrc = gst_element_factory_make("appsrc", "rtp-jpeg-appsrc");
  g_object_set(G_OBJECT(appsrc), "stream-type", 0, "format", GST_FORMAT_TIME, nullptr);

  // Create a capsfilter element to set the caps for the H.264 stream
  GstElement *capsfilter = gst_element_factory_make("capsfilter", "rtp-jpeg-capsfilter");
  // Raw RGB caps
  GstCaps *caps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "RGB", "width", G_TYPE_INT,
                                      GetEgressPort().width, "height", G_TYPE_INT, GetEgressPort().height, "framerate",
                                      GST_TYPE_FRACTION, GetEgressPort().framerate, 1, nullptr);
  g_object_set(G_OBJECT(capsfilter), "caps", caps, nullptr);

  // Convert the video colourspace
  GstElement *videoconvert = gst_element_factory_make("videoconvert", "rtp-h265-videoconvert");

  //
  GstElement *capsfilter2 = gst_element_factory_make("capsfilter", "rtp-jpeg-capsfilter2");
  // Raw RGB caps
  GstCaps *caps2 = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "I420", nullptr);
  g_object_set(G_OBJECT(capsfilter2), "caps2", caps2, nullptr);

  // Create a h265enc element to decode the H.264 stream
  GstElement *h265enc = gst_element_factory_make("jpegenc", "rtp-jpeg-enc");

  // RTP payloader
  GstElement *rtp264pay = gst_element_factory_make("rtpjpegpay", "rtp-jpeg-payloader");

  // Create a udpsink element to stream over ethernet
  GstElement *udpsink = gst_element_factory_make("udpsink", "rtp-jpeg-udpsink");
  g_object_set(G_OBJECT(udpsink), "host", GetEgressPort().hostname.c_str(), "port", GetEgressPort().port_no, nullptr);

  // Add all elements to the pipeline
  gst_bin_add_many(GST_BIN(pipeline_), appsrc, capsfilter, videoconvert, capsfilter2, h265enc, rtp264pay, udpsink,
                   nullptr);

  // Link the elements
  gst_element_link_many(appsrc, capsfilter, videoconvert, capsfilter2, h265enc, rtp264pay, udpsink, nullptr);

  return true;
}

void RtpJpegGstPayloader::Close() {
  // Stop the pipeline
  Stop();

  // Free resources
  gst_object_unref(pipeline_);
}

void RtpJpegGstPayloader::Start() {
  started_ = true;
  // Start the pipeline
  gst_element_set_state(pipeline_, GST_STATE_PLAYING);
}

void RtpJpegGstPayloader::Stop() {
  started_ = false;
  // Stop the pipeline
  gst_element_set_state(pipeline_, GST_STATE_NULL);
}

}  // namespace mediax::rtp::jpeg::gst
