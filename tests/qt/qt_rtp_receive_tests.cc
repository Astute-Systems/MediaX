#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <qt/QtRtpH264Depayloader.h>
#include <qt/QtRtpUncompressedDepayloader.h>
#include <qt/QtRtpUncompressedPayloader.h>

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

mediax::rtp::StreamInformation stream_info = {
    "test-session", "127.0.0.1", 5004, 480, 640, 25, ::mediax::rtp::ColourspaceType::kColourspaceRgb24};

bool running = false;

// Create a transmitter
void CreateTransmitter() {
  QByteArray frame;
  frame.resize(640 * 480 * 3);
  frame.fill(0xff);
  mediax::qt::QtRtpUncompressedPayloader rtp;
  rtp.setStreamInfo(stream_info);
  rtp.open();
  rtp.start();
  while (running) {
    emit rtp.sendFrame(&frame);
    QTest::qWait(40);
    QCoreApplication::processEvents();
  }
  rtp.stop();
  rtp.close();
}

void CreateTransmitter2() {
  QByteArray frame;
  frame.resize(640 * 480 * 3);
  frame.fill(0xff);
  mediax::qt::QtRtpH264Payloader rtp;
  rtp.setStreamInfo(stream_info);
  rtp.open();
  rtp.start();
  while (running) {
    emit rtp.sendFrame(&frame);
    QTest::qWait(40);
    QCoreApplication::processEvents();
  }
  rtp.stop();
  rtp.close();
}
TEST_F(QtRtpReceive, Constructors) {
  running = true;
  mediax::qt::QtRtpUncompressedDepayloader uncompressed_depayloader;
  mediax::qt::QtRtpH264Depayloader h264_depayloader;
  EXPECT_EQ(uncompressed_depayloader.getState(), ::mediax::rtp::StreamState::kClosed);
  EXPECT_EQ(h264_depayloader.getState(), ::mediax::rtp::StreamState::kClosed);
}

class UncompressedRecieve : public QObject {
  Q_OBJECT
 public:
  UncompressedRecieve() {  // Set the stream info

    uncompressed_depayloader.setStreamInfo(stream_info);

    // Connect signal to slot for newFrame
    connect(&uncompressed_depayloader, &mediax::qt::QtRtpUncompressedDepayloader::newFrame, this,
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
  void newFrame(QByteArray* frame) {
    // Do something with the frame
    m_count++;
    // std::cout << "Frame received: " << m_count << std::endl;
  }

 private:
  QByteArray frame;
  std::thread m_thread;
  mediax::qt::QtRtpUncompressedDepayloader uncompressed_depayloader;
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

    compressed_depayloader.setStreamInfo(stream_info);

    // Connect signal to slot for newFrame
    connect(&compressed_depayloader, &mediax::qt::QtRtpH264Depayloader::newFrame, this, &H264Recieve::newFrame);
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
  void newFrame(QByteArray* frame) {
    // Do something with the frame
    m_count++;
    // std::cout << "Frame received: " << m_count << std::endl;
  }

 private:
  QByteArray frame;
  std::thread m_thread;
  mediax::qt::QtRtpH264Depayloader compressed_depayloader;
  int m_count = 0;
};

TEST_F(QtRtpReceive, disabled_RtpH264) {
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