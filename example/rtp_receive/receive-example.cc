//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief A simple video receiver example
///
/// \file receive-example.cc
///

#include <cairo.h>
#include <cairo/cairo.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtk/gtk.h>

#include <iostream>
#include <memory>
#include <string>

#include "example.h"
#include "example_helpers.h"
#include "rtp/rtp.h"
#include "sap/sap_listener.h"
#include "version.h"

DEFINE_string(ipaddr, kIpAddressDefault, "the IP address of the transmit stream");
DEFINE_uint32(port, kPortDefault, "the port to use for the transmit stream");
DEFINE_uint32(height, kHeightDefault, "the height of the image");
DEFINE_uint32(width, kWidthDefault, "the width of the image");
DEFINE_string(session_name, kSessionName, "the SAP/SDP session name");
DEFINE_bool(wait_sap, false, "wait for SAP/SDP announcement");
DEFINE_bool(uncompressed, true, "Uncompressed video stream");
DEFINE_uint32(mode, 1,
              "The video mode (0-4)\n\t"
              "0 - Uncompressed RGB\n\t"
              "1 - Uncompressed YUV\n\t"
              "2 - Mono16\n\t"
              "3 - Mono8\n\t"
              "4 - H.264\n\t");

struct OnDrawData {
  std::string name;
  cairo_surface_t *surface;
  int32_t height;
  int32_t width;
  std::string ipaddr;
  uint16_t port;
};

std::shared_ptr<mediax::RtpDepayloader> rtp_;
std::shared_ptr<mediax::sap::SAPListener> sap_listener_;

static uint64_t m_frame_counter_ = 0;

gboolean on_draw(const GtkWidget *widget [[maybe_unused]], cairo_t *cr, gpointer user_data) {
  uint8_t *cpu_buffer;
  auto data = static_cast<OnDrawData *>(user_data);
  mediax::video::ColourSpaceCpu convert;

  // Overwrite line below to test the frame rate
  std::cout << "Frame=" << m_frame_counter_ << "\r";
  std::flush(std::cout);

  // Fill the surface with video data if available
  if (rtp_->Receive(&cpu_buffer, 80) == true) {
    unsigned char *surface_data = cairo_image_surface_get_data(data->surface);

    // Get the width and height of the surface
    int width = cairo_image_surface_get_width(data->surface);
    int height = cairo_image_surface_get_height(data->surface);

    // Check the colourspace
    if (auto format = cairo_image_surface_get_format(data->surface); format != CAIRO_FORMAT_RGB24) {
      LOG(ERROR) << "Unsupported format=" << format << "\n";
      return FALSE;
    }

    switch (FLAGS_mode) {
      case 0:
        convert.RgbToBgra(height, width, cpu_buffer, surface_data);
        break;
      case 1:
        convert.YuvToBgra(height, width, cpu_buffer, surface_data);
        break;
      case 2:
        convert.Mono16ToBgra(height, width, cpu_buffer, surface_data);
        break;
      case 3:
        convert.Mono8ToBgra(height, width, cpu_buffer, surface_data);
        break;
      case 4:
        convert.Nv12ToBgra(height, width, cpu_buffer, surface_data);
        break;
      default:
        LOG(ERROR) << "Unsupported mode=" << FLAGS_mode << "\n";
        break;
    }

    // Mark the surface as dirty to ensure the data is properly updated
    cairo_surface_mark_dirty(data->surface);
    cairo_set_source_surface(cr, data->surface, 0, 0);
    cairo_paint(cr);
  } else {
    // Timedout
    cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 24);
    cairo_move_to(cr, 20, 50);
    std::string no_stream = "No Stream " + data->name + ":" + data->ipaddr + ":" + std::to_string(data->port) + "\n";
    cairo_show_text(cr, no_stream.c_str());
  }
  m_frame_counter_++;
  return FALSE;
}

gboolean update_callback(gpointer user_data) {
  gtk_widget_queue_draw(GTK_WIDGET(user_data));
  return TRUE;
}

void ProcessVideo(mediax::ColourspaceType mode) {
  if ((mode == mediax::ColourspaceType::kColourspaceH264Part10) ||
      (mode == mediax::ColourspaceType::kColourspaceH264Part4)) {
    rtp_ = std::make_shared<mediax::h264::gst::vaapi::RtpH264GstVaapiDepayloader>();
  } else {
    rtp_ = std::make_shared<mediax::RtpUncompressedDepayloader>();
  }

  // Setup stream
  mediax::StreamInformation stream_information = {
      FLAGS_session_name, FLAGS_ipaddr, (uint16_t)FLAGS_port, FLAGS_height, FLAGS_width, 25, mode, false};
  if (FLAGS_wait_sap) {
    // Just give the stream name and wait for SAP/SDP announcement
    LOG(INFO) << "Example RTP streaming to " << FLAGS_session_name;
    sap_listener_ = std::make_shared<mediax::sap::SAPListener>();
    sap_listener_->Start();
    // Sleep for one second to allow the SAP listener to start
    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (sap_listener_->GetStreamInformation(FLAGS_session_name, &stream_information) == false) {
      LOG(ERROR) << "Could not get stream information, quitting";
      exit(1);
    }
    // Add SAP callback here
    rtp_->SetStreamInfo(stream_information);
  } else {
    LOG(INFO) << "Example RTP streaming to " << FLAGS_ipaddr.c_str() << ":" << FLAGS_port;
    rtp_->SetStreamInfo(stream_information);

    // We have all the information so we can request the ports open now. No need to wait for SAP/SDP
    if (!rtp_->Open()) {
      LOG(ERROR) << "Could not open stream, quitting";
      exit(1);
    }
  }

  rtp_->Start();
}

int main(int argc, char *argv[]) {
  gflags::SetVersionString(kVersion);
  gflags::SetUsageMessage(
      "Example RTP receiver\n"
      "Usage:\n"
      "  receive-example [OPTION]...\n"
      "Example:\n"
      "  receive-example -ipaddr=127.0.0.1 --port=5004 --height=480 --width=640\n");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  gtk_init(&argc, &argv);

  mediax::InitRtp(argc, argv);
  LOG(INFO) << "Example RTP streaming (" << FLAGS_width << "x" << FLAGS_height << " " << ModeToString(FLAGS_mode)
            << ") to " << FLAGS_ipaddr.c_str() << ":" << FLAGS_port;

  mediax::ColourspaceType video_mode = GetMode(FLAGS_mode);

  ProcessVideo(video_mode);

  // Create a new window
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_title(GTK_WINDOW(window), "receive-example");
  gtk_window_set_default_size(GTK_WINDOW(window), FLAGS_width, FLAGS_height);

  // Create a drawing area widget
  GtkWidget *drawing_area = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(window), drawing_area);

  // Create a surface and set it as the user data for the draw area
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, FLAGS_width, FLAGS_height);
  gtk_widget_set_size_request(drawing_area, FLAGS_width, FLAGS_height);
  g_object_set_data(G_OBJECT(drawing_area), "surface", surface);

  OnDrawData data = {
      FLAGS_session_name,  surface, static_cast<int>(FLAGS_height), static_cast<int>(FLAGS_width), FLAGS_ipaddr,
      (uint16_t)FLAGS_port};

  // Connect to the "draw" signal of the drawing area
  g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(on_draw), &data);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

  // Start the update timer
  g_timeout_add(40, update_callback, drawing_area);  // 25 Hz = 40ms interval

  gtk_widget_show_all(window);

  gtk_main();

  rtp_->Stop();

  mediax::RtpCleanup();

  LOG(INFO) << "Example terminated...";

  return 0;
}
