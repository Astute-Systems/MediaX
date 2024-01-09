
//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
///
/// \brief RTP streaming video class for H.264 DEF-STAN 00-82 video streams
///
/// Below is a sample pipeline to create video streams using GStreamer:
/// \code
/// gst-launch-1.0 -v udpsrc caps="application/x-rtp, media=(string)video, clock-rate=(int)90000,
/// encoding-name=(string)H264" ! rtph264depay ! h264parse ! queue ! nvh264dec ! caps="video/x-raw, format=RGB" !
/// videoconvert ! appsink
/// \endcode
///
/// \file rtp_h264_depayloader.cc
///

#include "h264/gst/nvidia/rtp_h264_depayloader.h"

#include <glog/logging.h>
#include <gst/gst.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "rtp/rtp_types.h"
#include "rtp/rtp_utils.h"

namespace mediax::rtp::h264::gst::nvidia {

RtpH264GstNvidiaDepayloader::RtpH264GstNvidiaDepayloader() {
  SetColourSpace(mediax::rtp::ColourspaceType::kColourspaceNv12);
}

RtpH264GstNvidiaDepayloader::~RtpH264GstNvidiaDepayloader() = default;

void RtpH264GstNvidiaDepayloader::SetStreamInfo(const ::mediax::rtp::StreamInformation &stream_information) {
  ::mediax::rtp::RtpPortType &stream = GetStream();
  stream.encoding = ::mediax::rtp::ColourspaceType::kColourspaceNv12;
  stream.height = stream_information.height;
  stream.width = stream_information.width;
  stream.framerate = stream_information.framerate;
  stream.name = stream_information.session_name;
  stream.hostname = stream_information.hostname;
  stream.port_no = stream_information.port;
  stream.settings_valid = true;
}

GstFlowReturn RtpH264GstNvidiaDepayloader::NewFrameCallback(GstAppSink *appsink, gpointer user_data) {
  gint width = 0;
  gint height = 0;
  auto depayloader = static_cast<RtpH264GstNvidiaDepayloader *>(user_data);

  // Pull the sample from the appsink
  GstSample *sample = gst_app_sink_pull_sample(appsink);

  // Get the buffer from the sample
  GstBuffer *buffer = gst_sample_get_buffer(sample);

  // Get the size of the buffer
  gsize size = gst_buffer_get_size(buffer);

  // Allocate memory for the frame data
  // guint8 *data = g_new(guint8, size);
  depayloader->GetBuffer().resize(size);

  // Get the buffer height and width
  const GstCaps *caps = gst_sample_get_caps(sample);
  const GstStructure *structure = gst_caps_get_structure(caps, 0);
  gst_structure_get_int(structure, "height", &height);
  gst_structure_get_int(structure, "width", &width);

  // Set the ColourspaceType
  if (const gchar *colorspace = gst_structure_get_string(structure, "format"); strncmp(colorspace, "UYVY", 4) == 0) {
    depayloader->SetColourSpace(mediax::rtp::ColourspaceType::kColourspaceYuv);
  } else if (strncmp(colorspace, "RGB", 3) == 0) {
    depayloader->SetColourSpace(mediax::rtp::ColourspaceType::kColourspaceRgb24);
  } else if (strncmp(colorspace, "NV12", 3) == 0) {
    depayloader->SetColourSpace(mediax::rtp::ColourspaceType::kColourspaceNv12);
  } else if (strncmp(colorspace, "RGBA", 4) == 0) {
    depayloader->SetColourSpace(mediax::rtp::ColourspaceType::kColourspaceRgba);
  } else {
    depayloader->SetColourSpace(mediax::rtp::ColourspaceType::kColourspaceUndefined);
    DLOG(WARNING) << "Unknown colourspace " << colorspace;
  }
  depayloader->SetHeight(height);
  depayloader->SetWidth(width);

  // Get a pointer to the video frame
  GstMapInfo map;
  gst_buffer_map(buffer, &map, GST_MAP_READ);

  // Copy the frame data
  std::copy(map.data, map.data + size, depayloader->GetBuffer().begin());

  // Unmap the buffer
  gst_buffer_unmap(buffer, &map);

  // Release the sample
  gst_sample_unref(sample);

  // Set good frame flag
  depayloader->NewFrame();

  return GST_FLOW_OK;
}

bool RtpH264GstNvidiaDepayloader::Open() {
  // Call the base class
  RtpDepayloader::Open();
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
  GstElement *nvh264dec = gst_element_factory_make("nvh264dec", "rtp-h264-nvh264dec");

  // Create a custom appsrc element to receive the H.264 stream
  GstElement *appsink = gst_element_factory_make("appsink", "rtp-h264-appsrc");

  // Set the callback function for the appsink
  GstAppSinkCallbacks callbacks = {.new_sample = RtpH264GstNvidiaDepayloader::NewFrameCallback};
  gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &callbacks, this, nullptr);

  // Add all elements to the pipeline
  gst_bin_add_many(GST_BIN(pipeline_), udpsrc, capsfilter, rtph264depay, h264parse, queue, nvh264dec, appsink, nullptr);

  // Link the elements
  gst_element_link_many(udpsrc, capsfilter, rtph264depay, h264parse, queue, nvh264dec, appsink, nullptr);

  return true;
}

void RtpH264GstNvidiaDepayloader::Start() {
  // Call the base class
  RtpDepayloader::Start();
  // Start the pipeline
  gst_element_set_state(pipeline_, GST_STATE_PLAYING);
}

void RtpH264GstNvidiaDepayloader::Stop() {
  // Call the base class
  RtpDepayloader::Stop();
  // Stop the pipeline
  gst_element_set_state(pipeline_, GST_STATE_NULL);
  std::cout << "Pipeline stopped" << std::endl;
  // Wait for the pipeline to finish
  GstBus *bus = gst_element_get_bus(pipeline_);

  // Free resources
  gst_object_unref(bus);
}

void RtpH264GstNvidiaDepayloader::Close() {
  // Call the base class
  RtpDepayloader::Close();
  // Destroy the pipeline
  gst_object_unref(pipeline_);
}

bool RtpH264GstNvidiaDepayloader::Receive(::mediax::rtp::RtpFrameData *data, int32_t timeout) {
  auto start_time = std::chrono::high_resolution_clock::now();

  // Dont start a new thread if a frame is available just return it
  data->resolution.height = GetHeight();
  data->resolution.width = GetWidth();
  data->encoding = GetColourSpace();
  data->cpu_buffer = GetBuffer().data();
  while (!new_rx_frame_) {
    // Check timeout
    if (auto elapsed = std::chrono::high_resolution_clock::now() - start_time;
        elapsed > std::chrono::milliseconds(timeout)) {
      // Blank the buffer, no data
      memset(GetBuffer().data(), 0, GetBuffer().size());
      return false;
    }
    // Sleep 1ms and wait for a new frame
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  new_rx_frame_ = false;
  return true;
}

void RtpH264GstNvidiaDepayloader::Callback(::mediax::rtp::RtpFrameData frame) const {
  if (GetState() == ::mediax::rtp::StreamState::kStarted) {
    GetCallback()(static_cast<const RtpDepayloader &>(*this), frame);
  }
}

void RtpH264GstNvidiaDepayloader::NewFrame() { new_rx_frame_ = true; }

}  // namespace mediax::rtp::h264::gst::nvidia
