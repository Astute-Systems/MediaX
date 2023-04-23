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

#include <cairomm/cairomm.h>
#include <gflags/gflags.h>
#include <gtkmm.h>

#include <cstdint>
#include <iostream>
#include <vector>

#include "rtp_stream.h"

DEFINE_string(ipaddr, "239.192.1.1", "the IP address of the transmit stream");
DEFINE_int32(port, 5004, "the port to use for the transmit stream");
DEFINE_int32(height, 480, "the height of the image");
DEFINE_int32(width, 640, "the width of the image");

class RGBBufferDrawingArea : public Gtk::DrawingArea {
 public:
  RGBBufferDrawingArea(int width, int height, const std::vector<uint8_t>& buffer)
      : m_width(width), m_height(height), m_buffer_(buffer), rtp_(RtpStream(height, width)) {
    // Setup RTP streaming class
    rtp_.RtpStreamIn("TestVideo1", FLAGS_ipaddr, (uint16_t)FLAGS_port);
    rtp_.Open();
  }

  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override {
    Gdk::Cairo::set_source_pixbuf(cr, getPixbuf(), 0, 0);
    cr->paint();
    return true;
  }

  Glib::RefPtr<Gdk::Pixbuf> getPixbuf() {
    auto pixbuf =
        Gdk::Pixbuf::create_from_data(m_buffer_.data(), Gdk::COLORSPACE_RGB, false, 8, m_width, m_height, m_width * 3);
    return pixbuf;
  }

  void update_buffer() {
    std::cout << "Frame = " << m_frame_counter_ << "\n";

    uint8_t* cpu_buffer;
    rtp_.Receive(&cpu_buffer);
    memset(m_buffer_.data(), 128, m_buffer_.size());  // Set buffer gray
    memcpy(m_buffer_.data(), cpu_buffer, m_buffer_.size());
    YuvToRgb(FLAGS_height, FLAGS_width, cpu_buffer, m_buffer_.data());
    m_frame_counter_++;
    queue_draw();
  }

 private:
  int m_width;
  int m_height;
  uint64_t m_frame_counter_ = 0;
  std::vector<uint8_t> m_buffer_;
  RtpStream rtp_;
};

class RGBBufferWindow : public Gtk::Window {
 public:
  RGBBufferWindow(int width, int height, const std::vector<uint8_t>& buffer) : m_drawing_area(width, height, buffer) {
    set_title("RGB Buffer Display");
    set_default_size(width, height);
    add(m_drawing_area);
    m_drawing_area.show();

    // Start the timer to update the buffer at 25 Hz (every 40 ms)
    m_connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &RGBBufferWindow::on_timeout), 40);
  }

  bool on_timeout() {
    m_drawing_area.update_buffer();
    return true;
  }

 private:
  RGBBufferDrawingArea m_drawing_area;
  sigc::connection m_connection;
};

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::cout << "Example RTP streaming to " << FLAGS_ipaddr.c_str() << ":" << FLAGS_port;

  auto app = Gtk::Application::create(argc, argv, "com.example.rgb_buffer_display");

  int width = FLAGS_width;
  int height = FLAGS_height;

  // Create a simple RGB buffer and fill white
  std::vector<uint8_t> buffer(width * height * 3, 255);

  RGBBufferWindow window(width, height, buffer);
  int ret = app->run(window);

  std::cout << "Example terminated...\n";
  return ret;
}