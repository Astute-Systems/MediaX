//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
// gst-launch-1.0 -v  videotestsrc ! video/x-raw, width=1280, height=720, framerate=1/50, media=video,
// encoding-name=RAW, payload=96, clock-rate=9600 ! x264enc ! h264parse ! rtph264pay ! udpsink host=239.192.1.1
// port=5004
///
/// \brief RTP streaming video class for H.264 DEF-STAN 00-82 video streams
///
/// \file rtp_h264_payloader.cc
///
// Example

#include "h264/gst/x264/rtp_h264_payloader.h"

#include <glog/logging.h>
#include <gst/app/gstappsrc.h>
#include <gst/gst.h>

namespace mediax::rtp::h264::gst::x264 {

RtpH264Gstx264Payloader::RtpH264Gstx264Payloader() = default;

RtpH264Gstx264Payloader::~RtpH264Gstx264Payloader() = default;

void RtpH264Gstx264Payloader::SetStreamInfo(const ::mediax::rtp::StreamInformation &stream_information) {
  GetEgressPort().encoding = stream_information.encoding;
  GetEgressPort().height = stream_information.height;
  GetEgressPort().width = stream_information.width;
  GetEgressPort().framerate = stream_information.framerate;
  GetEgressPort().name = stream_information.session_name;
  GetEgressPort().hostname = stream_information.hostname;
  GetEgressPort().port_no = stream_information.port;
  GetEgressPort().settings_valid = true;
}

int RtpH264Gstx264Payloader::Transmit(unsigned char *new_buffer, bool timeout) {
  if (!started_) {
    DLOG(ERROR) << "RTP H.264 payloader not started";
    return -1;
  }

  // Gstreamer send appsrc
  // Send a frame
  GstElement *appsrc = gst_bin_get_by_name(GST_BIN(pipeline_), "rtp-h264-appsrc");
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

  // Sync
  gst_element_sync_state_with_parent(appsrc);

  return 0;
}

bool RtpH264Gstx264Payloader::Open() {
  // Setup a gstreamer pipeline to decode H.264 with Intel x264I

  // Create a pipeline
  pipeline_ = gst_pipeline_new("rtp-h264-pipeline");
  // Create a custom appsrc element to receive the H.264 stream
  GstElement *appsrc = gst_element_factory_make("appsrc", "rtp-h264-appsrc");
  g_object_set(G_OBJECT(appsrc), "stream-type", 0, "format", GST_FORMAT_TIME, nullptr);

  // Create a capsfilter element to set the caps for the H.264 stream
  GstElement *capsfilter = gst_element_factory_make("capsfilter", "rtp-h264-capsfilter");
  // Raw RGB caps
  GstCaps *caps =
      gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "RGB", "width", G_TYPE_INT, GetEgressPort().width,
                          "height", G_TYPE_INT, GetEgressPort().height, "framerate", GST_TYPE_FRACTION,
                          GetEgressPort().framerate, 1, "media", G_TYPE_STRING, "video", "clock-rate", G_TYPE_INT,
                          90000, "encoding-name", G_TYPE_STRING, "RAW", "payload", G_TYPE_INT, 96, nullptr);
  g_object_set(G_OBJECT(capsfilter), "caps", caps, nullptr);

  // Convert the video colourspace
  GstElement *videoconvert = gst_element_factory_make("videoconvert", "rtp-h264-videoconvert");
  if (videoconvert == nullptr) {
    std::cerr << "No gst element called 'videoconvert'!\n";
    return false;
  }

  // Create a x264enc element to decode the H.264 stream
  GstElement *x264h264dec = gst_element_factory_make("x264enc", "rtp-h264-x264enc");
  if (x264h264dec == nullptr) {
    std::cerr << "No gst element called 'x264enc'!\n";
    return false;
  }

  // Set keyframe-period=1 max-bframes=0
  // g_object_set(G_OBJECT(x264h264dec), "keyframe-period", 1, "max-bframes", 0, nullptr);

  // RTP payloader
  GstElement *rtp264pay = gst_element_factory_make("rtph264pay", "rtp-h264-payloader");
  if (rtp264pay == nullptr) {
    std::cerr << "No gst element called 'rtph264pay'!\n";
    return false;
  }

  // Create a udpsink element to stream over ethernet
  GstElement *udpsink = gst_element_factory_make("udpsink", "rtp-h264-udpsink");
  if (udpsink == nullptr) {
    std::cerr << "No gst element called 'udpsink'!\n";
    return false;
  }
  g_object_set(G_OBJECT(udpsink), "host", GetEgressPort().hostname.c_str(), "port", GetEgressPort().port_no, nullptr);

  // Add all elements to the pipeline
  gst_bin_add_many(GST_BIN(pipeline_), appsrc, capsfilter, videoconvert, x264h264dec, rtp264pay, udpsink, nullptr);

  // Link the elements
  gst_element_link_many(appsrc, capsfilter, videoconvert, x264h264dec, rtp264pay, udpsink, nullptr);

  return true;
}

void RtpH264Gstx264Payloader::Close() {
  // Stop the pipeline
  Stop();

  // Free resources
  // gst_object_unref(bus);
  gst_object_unref(pipeline_);
}

void RtpH264Gstx264Payloader::Start() {
  started_ = true;
  // Start the pipeline
  gst_element_set_state(pipeline_, GST_STATE_PLAYING);
}

void RtpH264Gstx264Payloader::Stop() {
  started_ = false;
  // Stop the pipeline
  gst_element_set_state(pipeline_, GST_STATE_NULL);
}

}  // namespace mediax::rtp::h264::gst::x264
