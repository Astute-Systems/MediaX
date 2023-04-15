#include <cairomm/cairomm.h>
#include <gflags/gflags.h>
#include <gtkmm.h>

#include <cstdint>
#include <vector>

#include "rtp_stream.h"

DEFINE_string(ipaddr, "239.192.1.1", "the IP address of the transmit stream");
DEFINE_int32(port, 5004, "the port to use for the transmit stream");
DEFINE_int32(height, 480, "the height of the image");
DEFINE_int32(width, 640, "the width of the image");

class RGBBufferDrawingArea : public Gtk::DrawingArea {
 public:
  RGBBufferDrawingArea(int width, int height, const std::vector<uint8_t>& buffer)
      : m_width(width), m_height(height), m_buffer(buffer), rtp_(RtpStream(height, width)) {
    // Setup RTP streaming class

    rtp_.RtpStreamIn(FLAGS_ipaddr, FLAGS_port);
    rtp_.Open();
  }

  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override {
    Gdk::Cairo::set_source_pixbuf(cr, getPixbuf(), 0, 0);
    cr->paint();
    return true;
  }

  Glib::RefPtr<Gdk::Pixbuf> getPixbuf() {
    auto pixbuf =
        Gdk::Pixbuf::create_from_data(m_buffer.data(), Gdk::COLORSPACE_RGB, false, 8, m_width, m_height, m_width * 3);
    return pixbuf;
  }

  void update_buffer() {
    // Update the buffer with a simple color cycling effect
    for (int i = 0; i < m_height; ++i) {
      for (int j = 0; j < m_width; ++j) {
        int idx = (i * m_width + j) * 3;
        m_buffer[idx] = (m_buffer[idx] + 1) % 256;
        m_buffer[idx + 1] = (m_buffer[idx + 1] + 1) % 256;
        m_buffer[idx + 2] = (m_buffer[idx + 2] + 1) % 256;
      }
    }
    rtp_.Receive((uint8_t*)m_buffer.data());

    queue_draw();
  }

 private:
  int m_width;
  int m_height;
  std::vector<uint8_t> m_buffer;
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

  auto app = Gtk::Application::create(argc, argv, "com.example.rgb_buffer_display");

  int width = FLAGS_width;
  int height = FLAGS_height;

  // Create a simple RGB buffer
  std::vector<uint8_t> buffer(width * height * 3, 0);
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      int idx = (i * width + j) * 3;
      buffer[idx] = i % 256;
      buffer[idx + 1] = j % 256;
      buffer[idx + 2] = (i + j) % 256;
    }
  }

  RGBBufferWindow window(width, height, buffer);
  return app->run(window);
}