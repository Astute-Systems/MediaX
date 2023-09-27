
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

#include <chrono>
#include <iostream>
#include <string>

namespace mediax {

RtpH264Depayloader::RtpH264Depayloader() = default;

void NewFrameCallback(GstElement *rtph264depay, guint8 *buffer, guint size, gpointer user_data) {
  std::cout << "New frame received" << std::endl;
  auto depayloader = reinterpret_cast<RtpH264Depayloader *>(user_data);
  depayloader->buffer_in_.resize(size);
  memcpy(depayloader->buffer_in_.data(), buffer, size);
  depayloader->new_rx_frame_ = true;
}

bool RtpH264Depayloader::Open() {
  // Open the pipeline
  // Create a pipeline
  pipeline_ = gst_pipeline_new("rtp-h264-pipeline");

  // Create a udpsrc element to receive the RTP stream
  GstElement *udpsrc = gst_element_factory_make("udpsrc", "rtp-h264-udpsrc");
  g_object_set(G_OBJECT(udpsrc), "port", GetPort(), nullptr);
  std::cout << "Port: " << GetPort() << std::endl;

  // Create a capsfilter element to set the caps for the RTP stream
  GstElement *capsfilter = gst_element_factory_make("capsfilter", "rtp-h264-capsfilter");
  GstCaps *caps =
      gst_caps_from_string("application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264");
  g_object_set(G_OBJECT(capsfilter), "caps", caps, nullptr);
  gst_caps_unref(caps);

  // Create a rtph264depay element to depayload the RTP stream
  GstElement *rtph264depay = gst_element_factory_make("rtph264depay", "rtp-h264-depay");

  // H.264 parse
  GstElement *h264parse = gst_element_factory_make("h264parse", "rtp-h264-h264parse");

  // Decode frame using vaapi
  GstElement *vaapidecode = gst_element_factory_make("vaapidecode", "rtp-h264-vaapidecode");

  // Set the new frame callback

  // Create a custom appsrc element to receive the H.264 stream
  GstElement *appsrc = gst_element_factory_make("appsrc", "rtp-h264-appsrc");
  g_object_set(G_OBJECT(appsrc), "stream-type", 0, "format", GST_FORMAT_TIME, nullptr);
  g_signal_connect(rtph264depay, "new-payload", G_CALLBACK(NewFrameCallback), this);

  // Add all elements to the pipeline
  gst_bin_add_many(GST_BIN(pipeline_), udpsrc, capsfilter, rtph264depay, h264parse, vaapidecode, appsrc, nullptr);

  // Link the elements
  gst_element_link_many(udpsrc, capsfilter, rtph264depay, h264parse, vaapidecode, appsrc, nullptr);

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

void RtpH264Depayloader::Close() {
  // Destroy the pipeline
  gst_object_unref(pipeline_);
}

bool RtpH264Depayloader::Receive(uint8_t **cpu, int32_t timeout) {
  auto now = std::chrono::high_resolution_clock::now();
  if (new_rx_frame_) {
    // Dont start a new thread if a frame is available just return it
    *cpu = buffer_in_.data();
    new_rx_frame_ = false;
    return true;
  } else {
    while (!new_rx_frame_) {
      // Check timeout
      if (auto elapsed = std::chrono::high_resolution_clock::now() - now; elapsed.count() > timeout) {
        return false;
      }
      // Wait for a new frame sleep 1ms
      std::chrono::milliseconds timespan(timeout);
    }
  }
  return true;
}

}  // namespace mediax
