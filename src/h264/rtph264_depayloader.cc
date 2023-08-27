
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

#include "rtph264_depayloader.h"

#include <gst/gst.h>

#include <iostream>
#include <string>

#include "rtph264_depayloader.h"

namespace mediax {

RtpH264rawDepayloader::RtpH264rawDepayloader() {
  // Initialize GStreamer
  gst_init(nullptr, nullptr);

  // Create a pipeline
  GstElement *pipeline = gst_pipeline_new("rtp-h264-pipeline");

  // Create a udpsrc element to receive the RTP stream
  GstElement *udpsrc = gst_element_factory_make("udpsrc", "rtp-h264-udpsrc");
  g_object_set(G_OBJECT(udpsrc), "port", 5000, NULL);

  // Create a capsfilter element to set the caps for the RTP stream
  GstElement *capsfilter = gst_element_factory_make("capsfilter", "rtp-h264-capsfilter");
  GstCaps *caps =
      gst_caps_from_string("application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264");
  g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
  gst_caps_unref(caps);

  // Create a rtph264depay element to depayload the RTP stream
  GstElement *rtph264depay = gst_element_factory_make("rtph264depay", "rtp-h264-depay");

  // Create a custom appsrc element to receive the H.264 stream
  GstElement *appsrc = gst_element_factory_make("appsrc", "rtp-h264-appsrc");
  g_object_set(G_OBJECT(appsrc), "stream-type", 0, "format", GST_FORMAT_TIME, NULL);

  // Add all elements to the pipeline
  gst_bin_add_many(GST_BIN(pipeline), udpsrc, capsfilter, rtph264depay, appsrc, NULL);

  // Link the elements
  gst_element_link_many(udpsrc, capsfilter, rtph264depay, NULL);

  // Set the callback function for the appsrc element
  // GstAppSrcCallbacks callbacks = {NULL, NULL, NULL};
  // callbacks.need_data = [](GstAppSrc *appsrc, guint size, gpointer user_data) -> gboolean {
  //   RtpH264Payloader *payloader = static_cast<RtpH264Payloader *>(user_data);
  //   GstBuffer *buffer = payloader->GetNextFrame();
  //   if (buffer != NULL) {
  //     GstFlowReturn ret;
  //     g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
  //     gst_buffer_unref(buffer);
  //     return TRUE;
  //   } else {
  //     return FALSE;
  //   }
  // };
  // gst_app_src_set_callbacks(GST_APP_SRC(appsrc), &callbacks, &payloader, NULL);

  // Start the pipeline
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  // Wait for the pipeline to finish
  GstBus *bus = gst_element_get_bus(pipeline);
  GstMessage *msg =
      gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
  if (msg != NULL) {
    gst_message_unref(msg);
  }

  // Stop the pipeline
  gst_element_set_state(pipeline, GST_STATE_NULL);

  // Free resources
  gst_object_unref(bus);
  gst_object_unref(pipeline);
}

void RtpH264rawDepayloader::Start() {
  // TODO (Ross) : Yet to implement
}

void RtpH264rawDepayloader::Stop() {
  // TODO (Ross) : Yet to implement
}

void RtpH264rawDepayloader::Close() const {
  // TODO (Ross) : Yet to implement
}

}  // namespace mediax
