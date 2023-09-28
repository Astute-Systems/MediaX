
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
/// gst-launch-1.0 -v udpsrc caps="application/x-rtp, media=(string)video, clock-rate=(int)90000,
/// encoding-name=(string)H264" ! rtph264depay ! h264parse ! queue ! vaapih264dec ! caps="video/x-raw, format=RGB" !
/// videoconvert ! appsink
///
/// \file rtph264_depayloader.cc
///

#include "h264/gst/rtph264_depayloader.h"

#include <gst/app/gstappsink.h>
#include <gst/gst.h>

#include <chrono>
#include <iostream>
#include <string>

namespace mediax {

RtpH264Depayloader::RtpH264Depayloader() = default;

GstFlowReturn NewFrameCallback(GstAppSink *appsink, gpointer user_data) {
  RtpH264Depayloader *depayloader = static_cast<RtpH264Depayloader *>(user_data);

  // Pull the sample from the appsink
  GstSample *sample = gst_app_sink_pull_sample(appsink);

  // Get the buffer from the sample
  GstBuffer *buffer = gst_sample_get_buffer(sample);

  // Get the size of the buffer
  gsize size = gst_buffer_get_size(buffer);
  std::cout << "New frame callback size" << size << std::endl;

  // Allocate memory for the frame data
  // guint8 *data = g_new(guint8, size);
  depayloader->buffer_in_.resize(size);

  // Copy the data from the buffer to the allocated memory
  gst_buffer_extract(buffer, 0, reinterpret_cast<guint8 *>(depayloader->buffer_in_.data()), size);

  // Release the sample
  gst_sample_unref(sample);

  // Set good frame flag
  depayloader->new_rx_frame_ = true;

  return GST_FLOW_OK;
}

bool RtpH264Depayloader::Open() {
  // Open the pipeline
  // Create a pipeline
  pipeline_ = gst_pipeline_new("rtp-h264-pipeline");

  // Create a udpsrc element to receive the RTP stream
  GstElement *udpsrc = gst_element_factory_make("udpsrc", "rtp-h264-udpsrc");
  g_object_set(G_OBJECT(udpsrc), "port", GetPort(), nullptr);

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

  // Queue
  GstElement *queue = gst_element_factory_make("queue", "rtp-h264-queue");

  // Decode frame using vaapi
  GstElement *vaapih264dec = gst_element_factory_make("vaapih264dec", "rtp-h264-vaapih264dec");

  // Set the new frame callback

  // Create a custom appsrc element to receive the H.264 stream
  GstElement *appsink = gst_element_factory_make("appsink", "rtp-h264-appsrc");
  // Set the callback function for the appsink
  GstAppSinkCallbacks callbacks = {.new_sample = NewFrameCallback};
  gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &callbacks, this, NULL);

  // Add all elements to the pipeline
  gst_bin_add_many(GST_BIN(pipeline_), udpsrc, capsfilter, rtph264depay, h264parse, queue, vaapih264dec, appsink,
                   nullptr);

  // Link the elements
  gst_element_link_many(udpsrc, capsfilter, rtph264depay, h264parse, queue, vaapih264dec, appsink, nullptr);

  return true;
}

void RtpH264Depayloader::Start() {
  // Start the pipeline
  gst_element_set_state(pipeline_, GST_STATE_PLAYING);
}

void RtpH264Depayloader::Stop() {
  // Stop the pipeline
  gst_element_set_state(pipeline_, GST_STATE_NULL);
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
}

void RtpH264Depayloader::Close() {
  // Destroy the pipeline
  gst_object_unref(pipeline_);
}

bool RtpH264Depayloader::Receive(uint8_t **cpu, int32_t timeout) {
  auto start_time = std::chrono::high_resolution_clock::now();
  if (new_rx_frame_) {
    // Dont start a new thread if a frame is available just return it
    *cpu = buffer_in_.data();
    new_rx_frame_ = false;
    return true;
  } else {
    while (!new_rx_frame_) {
      // Check timeout
      if (auto elapsed = std::chrono::high_resolution_clock::now() - start_time;
          elapsed > std::chrono::milliseconds(timeout)) {
        std::cout << "Timeout" << std::endl;
        return false;
      }
      // Wait for a new frame sleep 1ms
      std::chrono::milliseconds timespan(timeout);
    }
  }
  return true;
}

}  // namespace mediax
