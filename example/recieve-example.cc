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

unsigned char *buffer = nullptr;  // Global variable to hold the RGB buffer

static RtpStream rtp_;

cairo_surface_t *create_rgb_surface(int width, int height) {
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
  unsigned char *data = cairo_image_surface_get_data(surface);
  int stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, width);

  // Fill the surface with RGB data
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      uint32_t pixel = (255 << 16) | (0 << 8) | 0;  // Red: 255, Green: 0, Blue: 0
      *((uint32_t *)(data + y * stride) + x) = pixel;
    }
  }

  // Mark the surface as dirty to ensure the data is properly updated
  cairo_surface_mark_dirty(surface);

  return surface;
}

static uint64_t m_frame_counter_ = 0;

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
  uint8_t *cpu_buffer;

  // Fill the surface with video data if available
  if (rtp_.Receive(&cpu_buffer, 60) == true) {
    cairo_surface_t *surface = static_cast<cairo_surface_t *>(user_data);
    unsigned char *data = cairo_image_surface_get_data(surface);
    // Get the width and height of the surface
    int width = cairo_image_surface_get_width(surface);
    int height = cairo_image_surface_get_height(surface);
    YuvToRgba(height, width, cpu_buffer, data);

    // Mark the surface as dirty to ensure the data is properly updated
    cairo_surface_mark_dirty(surface);
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
  } else {
    // Timedout
    cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 24);
    cairo_move_to(cr, 20, 50);
    cairo_show_text(cr, "No Stream");
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

  std::cout << "Example RTP streaming to " << FLAGS_ipaddr.c_str() << ":" << FLAGS_port << "\n";

  gtk_init(&argc, &argv);

  // Setup stream
  rtp_.RtpStreamIn("TestVideo1", FLAGS_height, FLAGS_width, FLAGS_ipaddr, (uint16_t)FLAGS_port);
  if (!rtp_.Open()) {
    std::cerr << "Could not open stream, quitting";
    return -1;
  }

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

  // Connect to the "draw" signal of the drawing area
  g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(on_draw), surface);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // Start the update timer
  g_timeout_add(40, update_callback, drawing_area);  // 25 Hz = 40ms interval

  gtk_widget_show_all(window);

  gtk_main();

  // Clean up
  if (buffer != NULL) {
    free(buffer);
  }

  return 0;
}
