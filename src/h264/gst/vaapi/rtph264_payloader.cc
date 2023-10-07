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
/// \file rtph264_payloader.cc
///

#include "h264/gst/vaapi/rtph264_payloader.h"

#include "h264/gst/vaapi/rtph264_depayloader.h"

namespace mediax::gst::vaapi {

RtpH264Payloader::RtpH264Payloader() = default;

RtpH264Payloader::~RtpH264Payloader() = default;

void RtpH264Payloader::SetStreamInfo(std::string_view name, ColourspaceType encoding, uint32_t height, uint32_t width,
                                     std::string_view hostname, const uint32_t portno) {
  egress_.encoding = encoding;
  egress_.height = height;
  egress_.width = width;
  egress_.framerate = 25;
  egress_.name = name;
  egress_.hostname = hostname;
  egress_.port_no = portno;
}

int RtpH264Payloader::Transmit(unsigned char *, bool) { return 0; }

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

  // Create a vaapidecode element to decode the H.264 stream
  GstElement *vaapidecode = gst_element_factory_make("vaapidecode", "rtp-h264-vaapidecode");

  // Create a vaapisink element to display the decoded H.264 stream
  GstElement *vaapisink = gst_element_factory_make("vaapisink", "rtp-h264-vaapisink");
  g_object_set(G_OBJECT(vaapisink), "sync", false, nullptr);

  // Add all elements to the pipeline
  gst_bin_add_many(GST_BIN(pipeline_), appsrc, capsfilter, vaapidecode, vaapisink, nullptr);

  // Link the elements
  gst_element_link_many(appsrc, capsfilter, vaapidecode, vaapisink, nullptr);

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

void RtpH264Payloader::Stop() {
  // Stop the pipeline
  gst_element_set_state(pipeline_, GST_STATE_NULL);
}

}  // namespace mediax::gst::vaapi
