
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

#include "h264/gst/rtph264_depayloader.h"

#include <gst/gst.h>

#include <iostream>
#include <string>

namespace mediax {

RtpH264Depayloader::RtpH264Depayloader() = default;

bool RtpH264Depayloader::Open() {
  // Open the pipeline
  // Create a pipeline
  pipeline_ = gst_pipeline_new("rtp-h264-pipeline");

  // Create a udpsrc element to receive the RTP stream
  GstElement *udpsrc = gst_element_factory_make("udpsrc", "rtp-h264-udpsrc");
  g_object_set(G_OBJECT(udpsrc), "port", 5000, nullptr);

  // Create a capsfilter element to set the caps for the RTP stream
  GstElement *capsfilter = gst_element_factory_make("capsfilter", "rtp-h264-capsfilter");
  GstCaps *caps =
      gst_caps_from_string("application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264");
  g_object_set(G_OBJECT(capsfilter), "caps", caps, nullptr);
  gst_caps_unref(caps);

  // Create a rtph264depay element to depayload the RTP stream
  GstElement *rtph264depay = gst_element_factory_make("rtph264depay", "rtp-h264-depay");

  // Create a custom appsrc element to receive the H.264 stream
  GstElement *appsrc = gst_element_factory_make("appsrc", "rtp-h264-appsrc");
  g_object_set(G_OBJECT(appsrc), "stream-type", 0, "format", GST_FORMAT_TIME, nullptr);

  // Add all elements to the pipeline
  gst_bin_add_many(GST_BIN(pipeline_), udpsrc, capsfilter, rtph264depay, appsrc, nullptr);

  // Link the elements
  gst_element_link_many(udpsrc, capsfilter, rtph264depay, nullptr);

  // Start the pipeline
  gst_element_set_state(pipeline_, GST_STATE_PLAYING);

  // Wait for the pipeline to finish
  GstBus *bus = gst_element_get_bus(pipeline_);

  if (GstMessage *msg =
          gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
      msg != nullptr) {
    gst_message_unref(msg);
  }

  // Stop the pipeline
  gst_element_set_state(pipeline_, GST_STATE_NULL);

  // Free resources
  gst_object_unref(bus);
  gst_object_unref(pipeline_);

  return true;
}

void RtpH264Depayloader::Start() {
  // Start the pipeline
  gst_element_set_state(pipeline_, GST_STATE_PLAYING);
}

void RtpH264Depayloader::Stop() {
  // Stop the pipeline
  gst_element_set_state(pipeline_, GST_STATE_NULL);
}

void RtpH264Depayloader::Close() const {
  // Destroy the pipeline
  gst_object_unref(pipeline_);
}

}  // namespace mediax
