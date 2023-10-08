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
/// \file rtp_h264_payloader.cc
///

#include "h264/gst/vaapi/rtp_h264_payloader.h"

#include <gst/app/gstappsrc.h>
#include <gst/gst.h>

#include "rtp/rtp.h"

namespace mediax::h264::gst::vaapi {

GstFlowReturn SourceFrameCallback(GstAppSink *appsink, gpointer user_data) {
  // Get the payloader object
  RtpH264Payloader *payloader = reinterpret_cast<RtpH264Payloader *>(user_data);

  // Get the sample from the appsink
  GstSample *sample = gst_app_sink_pull_sample(appsink);

  // Get the buffer from the sample
  GstBuffer *buffer = gst_sample_get_buffer(sample);

  // Get the caps from the sample
  GstCaps *caps = gst_sample_get_caps(sample);

  // Get the width and height from the caps
  GstStructure *structure = gst_caps_get_structure(caps, 0);
  int width, height;
  gst_structure_get_int(structure, "width", &width);
  gst_structure_get_int(structure, "height", &height);

  // Get the data from the buffer
  GstMapInfo map;
  gst_buffer_map(buffer, &map, GST_MAP_READ);

  // TODO(Ross Newman): Send video

  // Unmap the buffer
  gst_buffer_unmap(buffer, &map);

  // Unref the sample
  gst_sample_unref(sample);

  return GST_FLOW_OK;
}

RtpH264Payloader::RtpH264Payloader() = default;

RtpH264Payloader::~RtpH264Payloader() = default;

void RtpH264Payloader::SetStreamInfo(const ::mediax::StreamInformation &stream_information) {
  egress_.encoding = stream_information.encoding;
  egress_.height = stream_information.height;
  egress_.width = stream_information.width;
  egress_.framerate = stream_information.framerate;
  egress_.name = stream_information.session_name;
  egress_.hostname = stream_information.hostname;
  egress_.port_no = stream_information.port;
  egress_.settings_valid = true;
}

bool RtpH264Payloader::Open() {
  // Setup a gstreamer pipeline to decode H.264 with Intel VAAPI

  // Create a pipeline
  pipeline_ = gst_pipeline_new("rtp-h264-pipeline");

  // Create a custom appsrc element to receive the H.264 stream
  GstElement *appsrc = gst_element_factory_make("appsrc", "rtp-h264-appsrc");
  g_object_set(G_OBJECT(appsrc), "stream-type", 0, "format", GST_FORMAT_TIME, nullptr);

  // Create a capsfilter element to set the caps for the H.264 stream
  GstElement *capsfilter = gst_element_factory_make("capsfilter", "rtp-h264-capsfilter");
  GstCaps *caps =
      gst_caps_from_string("video/x-h264, stream-format=byte-stream, alignment=au, profile=high, level=4.1");
  g_object_set(G_OBJECT(capsfilter), "caps", caps, nullptr);
  gst_caps_unref(caps);

  // Create a nvh264enc element to decode the H.264 stream
  GstElement *nvh264enc = gst_element_factory_make("nvh264enc", "rtp-h264-nvh264enc");

  // Create an RTP payloader for the H.264 RTP stream
  GstElement *rtph264pay = gst_element_factory_make("rtph264pay", "rtp-h264-rtph264pay");

  // Create a udp sink to transmit the RTP stream
  GstElement *udpsink = gst_element_factory_make("udpsink", "rtp-h264-udpsink");
  g_object_set(G_OBJECT(udpsink), "host", egress_.hostname.c_str(), "port", egress_.port_no, nullptr);

  // Add all elements to the pipeline
  gst_bin_add_many(GST_BIN(pipeline_), appsrc, capsfilter, nvh264enc, rtph264pay, udpsink, nullptr);

  // Link the elements
  gst_element_link_many(appsrc, capsfilter, nvh264enc, rtph264pay, udpsink, nullptr);

  // Setup the appsrc element
  // g_object_set(G_OBJECT(appsrc), "emit-signals", true, "do-timestamp", true, nullptr);
  // g_signal_connect(appsrc, "need-data", G_CALLBACK(SourceFrameCallback), this);

  // Trigger callback every 40 ms
  gst_base_sink_set_sync(GST_BASE_SINK(appsrc), true);

  // Set the caps on the appsrc element for RAW RGB video
  GstCaps *appsrc_caps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "RGB", "width", G_TYPE_INT, 640,
                                             "height", G_TYPE_INT, 480, "framerate", GST_TYPE_FRACTION, 25, 1, nullptr);
  return true;
}

void RtpH264Payloader::Close() {
  // Stop the pipeline
  Stop();

  // Wait for the pipeline to finish
  GstBus *bus = gst_element_get_bus(pipeline_);
  if (GstMessage *msg =
          gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
      msg != nullptr) {
    gst_message_unref(msg);
  }

  // Free resources
  gst_object_unref(bus);
  gst_object_unref(pipeline_);
}

void RtpH264Payloader::Start() {
  // Start the pipeline
  gst_element_set_state(pipeline_, GST_STATE_PLAYING);
}

int RtpH264Payloader::Transmit(uint8_t *rgbframe, bool blocking) {
  // Get the appsrc element
  GstElement *appsrc = gst_bin_get_by_name(GST_BIN(pipeline_), "rtp-h264-appsrc");

  // Create a buffer from the RGB frame
  GstBuffer *buffer =
      gst_buffer_new_wrapped(rgbframe, egress_.width * egress_.height * mediax::BytesPerPixel(egress_.encoding));

  // Push the buffer to the appsrc element
  GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(appsrc), buffer);

  // Unref the appsrc element
  gst_object_unref(appsrc);

  // Return the result
  return ret;
}

void RtpH264Payloader::Stop() {
  // Stop the pipeline
  gst_element_set_state(pipeline_, GST_STATE_NULL);
}

}  // namespace mediax::h264::gst::vaapi
