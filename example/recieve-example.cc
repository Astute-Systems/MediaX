//
// Copyright (C) 2023 DefenceX PTY LTD. All rights reserved.
//
// This software is distributed under the included copyright license.
// Any redistribution or reproduction, in part or in whole, in any form or medium, is strictly prohibited without the
// prior written consent of DefenceX PTY LTD.
//
// For any inquiries or concerns, please contact:
// DefenceX PTY LTD
// Email: enquiries@defencex.ai
//
/// \file recieve-example.cc
///

#include <cairo.h>
#include <cairomm/cairomm.h>
#include <gflags/gflags.h>
#include <gtk/gtk.h>

#include <iostream>
#include <string>

#include "rtp_stream.h"

DEFINE_string(ipaddr, "239.192.1.1", "the IP address of the transmit stream");
DEFINE_int32(port, 5004, "the port to use for the transmit stream");
DEFINE_int32(height, 480, "the height of the image");
DEFINE_int32(width, 640, "the width of the image");
DEFINE_string(session_name, "TestVideo1", "the SAP/SDP session name");
DEFINE_bool(wait_sap, false, "wait for SAP/SDP announcement");

struct OnDrawData {
  std::string name;
  cairo_surface_t *surface;
  int32_t height;
  int32_t width;
  std::string ipaddr;
  uint16_t port;
};

unsigned char *buffer = nullptr;  // Global variable to hold the RGB buffer

static RtpStream rtp_;

static uint64_t m_frame_counter_ = 0;

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
  uint8_t *cpu_buffer;
  auto data = static_cast<OnDrawData *>(user_data);

  // Fill the surface with video data if available
  if (rtp_.Receive(&cpu_buffer, 80) == true) {
    unsigned char *surface_data = cairo_image_surface_get_data(data->surface);
    // Get the width and height of the surface
    int width = cairo_image_surface_get_width(data->surface);
    int height = cairo_image_surface_get_height(data->surface);
    YuvToRgba(height, width, cpu_buffer, surface_data);

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

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  gtk_init(&argc, &argv);

  // Setup stream
  if (FLAGS_wait_sap) {
    // Just give the stream name and wait for SAP/SDP announcement
    std::cout << "Example RTP streaming to " << FLAGS_session_name << "\n";
    rtp_.RtpStreamIn("TestVideo1");
  } else {
    std::cout << "Example RTP streaming to " << FLAGS_ipaddr.c_str() << ":" << FLAGS_port << "\n";
    rtp_.RtpStreamIn("TestVideo1", ColourspaceType::kColourspaceYuv, FLAGS_height, FLAGS_width, FLAGS_ipaddr,
                     (uint16_t)FLAGS_port);

    // We have all the information so we can request the ports open now. No need to wait for SAP/SDP
    if (!rtp_.Open()) {
      std::cerr << "Could not open stream, quitting";
      return -1;
    }
  }

  rtp_.Start();

  // Create a new window
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_title(GTK_WINDOW(window), "recieve-example");
  gtk_window_set_default_size(GTK_WINDOW(window), FLAGS_width, FLAGS_height);

  // Create a drawing area widget
  GtkWidget *drawing_area = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(window), drawing_area);

  // Create a surface and set it as the user data for the draw area
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, FLAGS_width, FLAGS_height);
  gtk_widget_set_size_request(drawing_area, FLAGS_width, FLAGS_height);
  g_object_set_data(G_OBJECT(drawing_area), "surface", surface);

  OnDrawData data = {"TestVideo1", surface, FLAGS_height, FLAGS_width, FLAGS_ipaddr, (uint16_t)FLAGS_port};

  // Connect to the "draw" signal of the drawing area
  g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(on_draw), &data);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // Start the update timer
  g_timeout_add(40, update_callback, drawing_area);  // 25 Hz = 40ms interval

  gtk_widget_show_all(window);

  gtk_main();

  rtp_.Stop();

  // Clean up
  if (buffer != NULL) {
    free(buffer);
  }

  return 0;
}
