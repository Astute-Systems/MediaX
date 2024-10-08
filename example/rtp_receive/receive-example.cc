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
#include <gtk/gtk.h>

#include <iostream>
#include <memory>
#include <string>

#include "example.h"
#include "example_helpers.h"
#include "rtp/rtp.h"
#include "sap/sap_listener.h"
#include "stdint.h"
#include "version.h"

DEFINE_string(ipaddr, kIpAddressDefault, "the IP address of the transmit stream");
DEFINE_uint32(port, kPortDefault, "the port to use for the transmit stream");
DEFINE_uint32(height, kHeightDefault, "the height of the image");
DEFINE_uint32(width, kWidthDefault, "the width of the image");
DEFINE_uint32(framerate, 25, "the frames per second");
DEFINE_string(session_name, kSessionName, "the SAP/SDP session name");
DEFINE_bool(verbose, false, "For verbose output");
DEFINE_bool(wait_sap, false, "wait for SAP/SDP announcement");
DEFINE_bool(uncompressed, true, "Uncompressed video stream");
DEFINE_uint32(mode, 1,
              "The video mode (0-4)\n\t"
              "0 - Uncompressed RGB\n\t"
              "1 - Uncompressed YUV\n\t"
              "2 - Mono16\n\t"
#if GST_SUPPORTED
              "3 - Mono8\n\t"
              "4 - H.264\n\t"
              "5 - H.265\n\t");
#else
              "3 - Mono8\n\t");
#endif
DEFINE_uint32(num_frames, 0, "The number of frames to send");

#define CALLBACK 1

struct OnDrawData {
  cairo_t *cr;
  std::string name;
  cairo_surface_t *surface;
  int32_t height;
  int32_t width;
  std::string ipaddr;
  uint16_t port;
};

GtkWidget *drawing_area = nullptr;
class Receive {
 public:
  ///
  /// \brief The GTK on_draw callback
  ///
  /// \param widget the widget
  /// \param cr the cairo context
  /// \param user_data the user data
  /// \return gboolean true if successful
  ///
  static gboolean OnDraw(const GtkWidget *widget [[maybe_unused]], cairo_t *cr, gpointer user_data) {
    // Time the start of the draw
    auto start = std::chrono::high_resolution_clock::now();

    mediax::rtp::RtpFrameData frame_data;
    auto data = static_cast<OnDrawData *>(user_data);
    mediax::video::ColourSpaceCpu convert;

    // Fill the surface with video data if available
    if (rtp_->Receive(&frame_data, timeout_) == true) {
      unsigned char *surface_data = cairo_image_surface_get_data(data->surface);

      // Get the width and height of the surface
      int width = cairo_image_surface_get_width(data->surface);
      int height = cairo_image_surface_get_height(data->surface);

      // Check the colourspace
      if (auto format = cairo_image_surface_get_format(data->surface); format != CAIRO_FORMAT_RGB24) {
        std::cerr << "Unsupported format=" << format << "\n";
        return FALSE;
      }

      switch (frame_data.encoding) {
        case mediax::rtp::ColourspaceType::kColourspaceRgb24:
          convert.RgbToBgra(height, width, frame_data.cpu_buffer, surface_data);
          break;
        case mediax::rtp::ColourspaceType::kColourspaceYuv422:
          convert.YuvToBgra(height, width, frame_data.cpu_buffer, surface_data);
          break;
        case mediax::rtp::ColourspaceType::kColourspaceMono16:
          convert.Mono16ToBgra(height, width, frame_data.cpu_buffer, surface_data);
          break;
        case mediax::rtp::ColourspaceType::kColourspaceMono8:
          convert.Mono8ToBgra(height, width, frame_data.cpu_buffer, surface_data);
          break;
        case mediax::rtp::ColourspaceType::kColourspaceNv12:
          convert.Nv12ToBgra(height, width, frame_data.cpu_buffer, surface_data);
          break;
        case mediax::rtp::ColourspaceType::kColourspaceRgba:
          convert.RgbaToBgra(height, width, frame_data.cpu_buffer, surface_data);
          break;
        default:
          std::cerr << "Unsupported mode=" << static_cast<int>(frame_data.encoding) << "\n";
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
      dropped_++;
    }

    if ((FLAGS_num_frames > 0) && (count_ >= FLAGS_num_frames)) {
      if (timeout_id_ != 0) g_source_remove(timeout_id_);

      gtk_main_quit();
    }

    // Overwrite line below to test the frame rate
    if (FLAGS_verbose) std::cout << "Frame=" << count_ << "\r";
    std::flush(std::cout);

    if (FLAGS_num_frames > 0) {
      count_++;
    }

    if (timeout_id_ != 0) g_source_remove(timeout_id_);
// If CALLBACK is not 1
#if !CALLBACK

    // Calculate the elapsed time in ms
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
    guint sleep_time = (1000 / FLAGS_framerate) - elapsed.count();
    if (elapsed.count() >= (1000 / FLAGS_framerate)) sleep_time = 1;

    // Update the g_timeout_add
    timeout_id_ = g_timeout_add(sleep_time, Receive::UpdateCallback,
                                drawing_area);  // framerate is in milliseconds
#else
    data->cr = cr;
    timeout_id_ = g_timeout_add(200, Receive::UpdateCallback,
                                drawing_area);  // framerate is in milliseconds

#endif
    return TRUE;
  }

  ///
  /// \brief The GTK update callback
  ///
  /// \param user_data the user data
  /// \return gboolean true if successful
  ///
  static gboolean UpdateCallback(gpointer user_data) {
    gtk_widget_queue_draw(GTK_WIDGET(user_data));
    return TRUE;
  }

#if CALLBACK
  ///
  /// \brief Rtp Callback to drive display
  ///
  /// \param depay the RTP Depayloader
  /// \param data the recieved video frame data
  ///
  static void RtpCallback(const mediax::rtp::RtpDepayloader &rtp [[maybe_unused]],
                          mediax::rtp::RtpFrameData frame [[maybe_unused]]) {
    // We dont need to render now its safe to call the Recieve function via the GTK callback
    gtk_widget_queue_draw(GTK_WIDGET(Receive::window));
    return;
  }
#endif

  ///
  /// \brief Start to process the incoming video stream
  ///
  /// \param mode the video mode
  ///
  static void SetupStream(mediax::rtp::ColourspaceType video_mode) {
    // Setup stream
    mediax::rtp::StreamInformation stream_information = {FLAGS_session_name, FLAGS_ipaddr, (uint16_t)FLAGS_port,
                                                         FLAGS_height,       FLAGS_width,  FLAGS_framerate,
                                                         video_mode,         false};
    if (FLAGS_wait_sap) {
      // Just give the stream name and wait for SAP/SDP announcement
      sap_listener_ = std::make_shared<mediax::sap::SapListener>();
      sap_listener_->Start();
      // Sleep for one second to allow the SAP listener to start
      std::this_thread::sleep_for(std::chrono::seconds(1));

      if (sap_listener_->GetStreamInformation(FLAGS_session_name, &stream_information) == false) {
        std::cerr << "Could not get stream information, quitting\n";
        exit(1);
      }
      // Add SAP callback here
      rtp_->SetStreamInfo(stream_information);
    } else {
      rtp_->SetStreamInfo(stream_information);
    }
  }

  static std::shared_ptr<mediax::rtp::RtpDepayloader> rtp_;
  static int32_t timeout_;
  static uint32_t count_;
  static uint32_t dropped_;
  static guint timeout_id_;
  static cairo_surface_t *surface;
  static GtkWidget *window;

 private:
  static std::shared_ptr<mediax::sap::SapListener> sap_listener_;

  static uint32_t frame_counter_;
};

std::shared_ptr<mediax::rtp::RtpDepayloader> Receive::rtp_;
std::shared_ptr<mediax::sap::SapListener> Receive::sap_listener_;
uint32_t Receive::frame_counter_ = 0;
uint32_t Receive::count_ = 1;
uint32_t Receive::dropped_ = 0;
int32_t Receive::timeout_ = 0;
guint Receive::timeout_id_ = 0;
cairo_surface_t *Receive::surface;
GtkWidget *Receive::window;

///
/// \brief The signal handler
///
/// \param signum the signal number
///
void signalHandler(int signum [[maybe_unused]]) {
  std::cout << "receive-example Interrupt signal (" << signum << ") received.\n";
  // GTK close
  gtk_main_quit();
  Receive::rtp_->Stop();
  Receive::rtp_->Close();
  mediax::RtpCleanup();
  exit(1);
}

///
/// \brief The main entry point
///
/// \param argc the argument count
/// \param argv the argument values
/// \return int the return code
///
int main(int argc, char *argv[]) {
  gflags::SetVersionString(mediax::kVersion);
  gflags::SetUsageMessage(
      "Example RTP receiver\n"
      "Usage:\n"
      "  rtp-receive [OPTION]...\n"
      "Example:\n"
      "  rtp-receive -ipaddr=127.0.0.1 --port=5004 --height=480 --width=640\n");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  gtk_init(&argc, &argv);

  mediax::InitRtp(argc, argv);
  std::cout << "Example RTP (Rx) streaming (" << FLAGS_width << "x" << FLAGS_height << " " << ModeToString(FLAGS_mode)
            << ") to " << FLAGS_ipaddr.c_str() << ":" << FLAGS_port << "@" << FLAGS_framerate << "Htz\n";

  // register signal SIGINT and signal handler
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  mediax::rtp::ColourspaceType video_mode = GetMode(FLAGS_mode);

  // Set the Rx timeout based on framerate
  Receive::timeout_ = (1000 / FLAGS_framerate) * 2;

  // Create a new window
  Receive::window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(Receive::window), GTK_WIN_POS_CENTER);
  gtk_window_set_title(GTK_WINDOW(Receive::window), "rtp-receive");
  gtk_window_set_default_size(GTK_WINDOW(Receive::window), FLAGS_width, FLAGS_height);

  // Create a drawing area widget
  drawing_area = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(Receive::window), drawing_area);

  // Create a surface and set it as the user data for the draw area
  Receive::surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, FLAGS_width, FLAGS_height);
  gtk_widget_set_size_request(drawing_area, FLAGS_width, FLAGS_height);
  g_object_set_data(G_OBJECT(drawing_area), "surface", Receive::surface);

  OnDrawData data = {nullptr,
                     FLAGS_session_name,
                     Receive::surface,
                     static_cast<int>(FLAGS_height),
                     static_cast<int>(FLAGS_width),
                     FLAGS_ipaddr,
                     (uint16_t)FLAGS_port};

  // Connect to the "draw" signal of the drawing area
  g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(Receive::OnDraw), &data);
  g_signal_connect(Receive::window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

// Create the required video stream
#if GST_SUPPORTED
  switch (video_mode) {
    default:  // Assume uncompressed
      Receive::rtp_ = std::make_shared<mediax::rtp::uncompressed::RtpUncompressedDepayloader>();
      break;
    case mediax::rtp::ColourspaceType::kColourspaceH264Part10:
    case mediax::rtp::ColourspaceType::kColourspaceH264Part4:
      Receive::rtp_ = std::make_shared<mediax::rtp::h264::gst::vaapi::RtpH264GstVaapiDepayloader>();
      break;
    case mediax::rtp::ColourspaceType::kColourspaceH265:
      Receive::rtp_ = std::make_shared<mediax::rtp::h265::gst::vaapi::RtpH265GstVaapiDepayloader>();
      break;
  }
#else
  Receive::rtp_ = std::make_shared<mediax::RtpUncompressedDepayloader>();
#endif

  // Start processing video
  Receive::SetupStream(video_mode);

#if CALLBACK
  // Setup the callback if we are event driven
  Receive::rtp_->RegisterCallback(Receive::RtpCallback);
#else

  // Start the update timer
  Receive::timeout_id_ =
      g_timeout_add(1000 / FLAGS_framerate, Receive::UpdateCallback, drawing_area);  // framerate is in milliseconds
#endif

  // We have all the information so we can request the ports open now. No need to wait for SAP/SDP
  if (!Receive::rtp_->Open()) {
    std::cerr << "Could not open stream, quitting\n";
    exit(1);
  }
  Receive::rtp_->Start();

  gtk_widget_show_all(Receive::window);

  gtk_main();

  Receive::rtp_->Stop();
  Receive::rtp_->Close();

  mediax::RtpCleanup();

  if (FLAGS_verbose) std::cout << "Recieved " << Receive::count_ << ", dropped " << Receive::dropped_ << "\n";
  std::cout << "RTP (Rx) Example terminated...\n";

  return 0;
}
