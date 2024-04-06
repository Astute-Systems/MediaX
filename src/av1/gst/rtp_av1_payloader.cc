//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video class for AV1 video streams
/// \code
/// gst-launch-1.0 -v videotestsrc ! videoconvert ! "video/x-raw, format=I420, framerate=1/25" ! av1enc ! av1parse ! rtpav1pay ! udpsink host=127.0.0.1 port=5004
/// \endcode
/// \file rtp_av1_payloader.cc
///

#include "av1/gst/rtp_av1_payloader.h"

#include <glog/logging.h>
#include <gst/app/gstappsrc.h>
#include <gst/gst.h>

#include "av1/gst/rtp_av1_depayloader.h"

namespace mediax::rtp::av1::gst {

RtpAv1GstPayloader::RtpAv1GstPayloader() = default;

RtpAv1GstPayloader::~RtpAv1GstPayloader() = default;

void RtpAv1GstPayloader::SetStreamInfo(const ::mediax::rtp::StreamInformation &stream_information) {
  GetEgressPort().encoding = stream_information.encoding;
  GetEgressPort().height = stream_information.height;
  GetEgressPort().width = stream_information.width;
  GetEgressPort().framerate = stream_information.framerate;
  GetEgressPort().name = stream_information.session_name;
  GetEgressPort().hostname = stream_information.hostname;
  GetEgressPort().port_no = stream_information.port;
  GetEgressPort().settings_valid = true;
}

int RtpAv1GstPayloader::Transmit(unsigned char *new_buffer, bool timeout) {
  if (!started_) {
    DLOG(ERROR) << "RTP JPEG payloader not started";
    return -1;
  }

  // Gstreamer send appsrc
  // Send a frame
  GstElement *appsrc = gst_bin_get_by_name(GST_BIN(pipeline_), "rtp-av1-appsrc");
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

bool RtpAv1GstPayloader::Open() {
  // Setup a gstreamer pipeline to decode H.264 with Intel

  // Create a pipeline
  pipeline_ = gst_pipeline_new("rtp-av1-pipeline");
  // Create a custom appsrc element to receive the H.264 stream
  GstElement *appsrc = gst_element_factory_make("appsrc", "rtp-av1-appsrc");
  g_object_set(G_OBJECT(appsrc), "stream-type", 0, "format", GST_FORMAT_TIME, nullptr);

  // Create a capsfilter element to set the caps for the H.264 stream
  GstElement *capsfilter = gst_element_factory_make("capsfilter", "rtp-av1-capsfilter");
  // Raw RGB caps
  GstCaps *caps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "RGB", "width", G_TYPE_INT,
                                      GetEgressPort().width, "height", G_TYPE_INT, GetEgressPort().height, "framerate",
                                      GST_TYPE_FRACTION, GetEgressPort().framerate, 1, nullptr);
  g_object_set(G_OBJECT(capsfilter), "caps", caps, nullptr);

  // Convert the video colourspace
  GstElement *videoconvert = gst_element_factory_make("videoconvert", "videoconvert");

  // Create a h265enc element to decode the H.264 stream
  GstElement *av1enc = gst_element_factory_make("av1enc", "rtp-av1-enc");
  if (!av1enc) {
    DLOG(ERROR) << "Failed to create av1enc element";
    return false;
  }

  // RTP Parser
  GstElement *rtpav1arse = gst_element_factory_make("av1parse", "rtp-av1-parser");
  if (!rtpav1arse) {
    DLOG(ERROR) << "Failed to create av1parse element";
    return false;
  }

  // RTP payloader
  GstElement *rtpav1ay = gst_element_factory_make("rtpav1pay", "rtp-av1-payloader");
  if (!rtpav1ay) {
    DLOG(ERROR) << "Failed to create rtpav1pay element";
    return false;
  }

  // Create a udpsink element to stream over ethernet
  GstElement *udpsink = gst_element_factory_make("udpsink", "rtp-av1-udpsink");
  g_object_set(G_OBJECT(udpsink), "host", GetEgressPort().hostname.c_str(), "port", GetEgressPort().port_no, nullptr);

  // Add all elements to the pipeline
  gst_bin_add_many(GST_BIN(pipeline_), appsrc, capsfilter, videoconvert, av1enc, rtpav1arse, rtpav1ay, udpsink,
                   nullptr);

  // Link the elements
  gst_element_link_many(appsrc, capsfilter, videoconvert, av1enc, rtpav1arse, rtpav1ay, udpsink, nullptr);

  return true;
}

void RtpAv1GstPayloader::Close() {
  // Stop the pipeline
  Stop();

  // Free resources
  gst_object_unref(pipeline_);
}

void RtpAv1GstPayloader::Start() {
  started_ = true;
  // Start the pipeline
  gst_element_set_state(pipeline_, GST_STATE_PLAYING);
}

void RtpAv1GstPayloader::Stop() {
  started_ = false;
  // Stop the pipeline
  gst_element_set_state(pipeline_, GST_STATE_NULL);
}

}  // namespace mediax::rtp::av1::gst
