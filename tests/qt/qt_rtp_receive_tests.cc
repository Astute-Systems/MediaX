#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <qt6/QtRtpUncompressedDepayloader.h>
#include <qt6/QtRtpUncompressedPayloader.h>
#include <qt6/QtRtpVaapiH264Depayloader.h>

#include "QObject"
#include "QTest"
#include "QVector"

class QtRtpReceive : public ::testing::Test {
 protected:
  int argc = 1;
  char* argv[1] = {(char*)"something"};

  void SetUp() override { app = new QCoreApplication(argc, argv); }

  void TearDown() override { delete app; }

  QCoreApplication* app;
};

mediax::rtp::StreamInformation stream_info_rgb24 = {
    "test-session", "127.0.0.1", 5004, 480, 640, 25, ::mediax::rtp::ColourspaceType::kColourspaceRgb24};

mediax::rtp::StreamInformation stream_info_h264 = {
    "test-session-h264", "127.0.0.1", 5004, 480, 640, 25, ::mediax::rtp::ColourspaceType::kColourspaceH264Part10};

bool running = false;

// Create a transmitter
void CreateTransmitter() {
  Frame frame;
  frame.video.resize(640 * 480 * 3);
  frame.video.fill(0xff);
  mediax::qt6::QtRtpUncompressedPayloader rtp;
  rtp.setStreamInfo(stream_info_rgb24);
  rtp.open();
  rtp.start();
  while (running) {
    emit rtp.sendFrame(frame);
    QTest::qWait(40);
    QCoreApplication::processEvents();
  }
  rtp.stop();
  rtp.close();
}

void CreateTransmitter2() {
  Frame frame;
  frame.video.resize(640 * 480 * 3);
  frame.video.fill(0xff);
  frame.height = 480;
  frame.width = 640;
  frame.encoding = ::mediax::rtp::ColourspaceType::kColourspaceH264Part10;
  mediax::qt6::QtRtpVaapiH264Payloader rtp;
  rtp.setStreamInfo(stream_info_h264);
  rtp.open();
  rtp.start();
  while (running) {
    rtp.transmit(&frame, false);
    QTest::qWait(40);
    QCoreApplication::processEvents();
  }
  rtp.stop();
  rtp.close();
}
TEST_F(QtRtpReceive, Constructors) {
  running = true;
  mediax::qt6::QtRtpUncompressedDepayloader uncompressed_depayloader;
  mediax::qt6::QtRtpVaapiH264Depayloader h264_depayloader;
  EXPECT_EQ(uncompressed_depayloader.getState(), ::mediax::rtp::StreamState::kClosed);
  EXPECT_EQ(h264_depayloader.getState(), ::mediax::rtp::StreamState::kClosed);
}

class UncompressedRecieve : public QObject {
  Q_OBJECT
 public:
  UncompressedRecieve() {  // Set the stream info

    uncompressed_depayloader.setStreamInfo(stream_info_rgb24);

    // Connect signal to slot for newFrame
    connect(&uncompressed_depayloader, &mediax::qt6::QtRtpUncompressedDepayloader::newFrame, this,
            &UncompressedRecieve::newFrame);
    uncompressed_depayloader.open();
    uncompressed_depayloader.start();
    // resize data
    frame.resize(640 * 480 * 3);
    frame.fill(0xff);
  }
  ~UncompressedRecieve() final {
    uncompressed_depayloader.stop();
    uncompressed_depayloader.close();
  }
  int getCount() const { return m_count; }
  mediax::rtp::StreamState getState() const { return uncompressed_depayloader.getState(); }

 public slots:
  void newFrame(Frame frame) {
    // Do something with the frame
    m_count++;
    EXPECT_EQ(frame.height, 480);
    EXPECT_EQ(frame.width, 640);
    EXPECT_EQ(frame.video.size(), 640 * 480 * 3);

    // std::cout << "Frame received: " << m_count << std::endl;
  }

 private:
  QByteArray frame;
  std::thread m_thread;
  mediax::qt6::QtRtpUncompressedDepayloader uncompressed_depayloader;
  int m_count = 0;
};

TEST_F(QtRtpReceive, RtpUncompressed) {
  running = true;
  UncompressedRecieve rtp;
  EXPECT_EQ(rtp.getState(), ::mediax::rtp::StreamState::kStarted);
  EXPECT_EQ(rtp.getCount(), 0);

  // Create a transmitter thread
  auto t = std::thread(CreateTransmitter);

  // sleep 1 second
  QTest::qWait(1000);
  running = false;

  if (t.joinable()) t.join();

  EXPECT_NE(rtp.getCount(), 0);
}

class H264Recieve : public QObject {
  Q_OBJECT
 public:
  H264Recieve() {  // Set the stream info

    // This should be set from the stream
    stream_info_h264.height = 0;
    stream_info_h264.width = 0;

    compressed_depayloader.setStreamInfo(stream_info_h264);

    // Connect signal to slot for newFrame
    connect(&compressed_depayloader, &mediax::qt6::QtRtpVaapiH264Depayloader::newFrame, this, &H264Recieve::newFrame);
    compressed_depayloader.open();
    compressed_depayloader.start();
    // resize data
    frame.resize(640 * 480 * 3);
    frame.fill(0xff);
  }
  ~H264Recieve() final {
    compressed_depayloader.stop();
    compressed_depayloader.close();
  }
  int getCount() const { return m_count; }
  mediax::rtp::StreamState getState() const { return compressed_depayloader.getState(); }

 public slots:
  void newFrame(Frame frame) {
    // Do something with the frame
    m_count++;
    EXPECT_EQ(frame.height, 480);
    EXPECT_EQ(frame.width, 640);
    EXPECT_EQ(frame.video.size(), 640 * 480 * 1.5);

    std::cout << "Frame received: " << m_count << " Size: " << frame.video.size() << std::endl;
  }

 private:
  QByteArray frame;
  std::thread m_thread;
  mediax::qt6::QtRtpVaapiH264Depayloader compressed_depayloader;
  int m_count = 0;
};

//
TEST_F(QtRtpReceive, DISABLED_RtpH264) {
  running = true;
  H264Recieve rtp;
  EXPECT_EQ(rtp.getState(), ::mediax::rtp::StreamState::kStarted);
  EXPECT_EQ(rtp.getCount(), 0);

  // Create a transmitter thread
  auto t = std::thread(CreateTransmitter2);

  // sleep 1 second
  QTest::qWait(1000);
  running = false;

  if (t.joinable()) t.join();

  EXPECT_NE(rtp.getCount(), 0);
}

#include "qt_rtp_receive_tests.moc"