#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <qt6/QtRtpH264Payloader.h>
#include <qt6/QtRtpUncompressedPayloader.h>
// QTest
#include <QTest>

class QtTest : public QObject {
  Q_OBJECT

 public:
  QtTest();

  ~QtTest() {
    rtp.stop();
    rtp.close();
  }

 signals:
  void newFrame(Frame frame);

 public:
  void sendFrame() {
    // Create a frame of RGB pixels in QByteArray
    Frame frame;
    frame.video.resize(640 * 480 * 3);
    frame.video.fill(0xff);
    frame.height = 480;
    frame.width = 640;
    frame.encoding = ::mediax::rtp::ColourspaceType::kColourspaceRgb24;

    // Send the frame to the payloader
    emit newFrame(frame);
  }

 private:
  mediax::qt6::QtRtpH264Payloader rtp;
};

QtTest::QtTest() {
  mediax::rtp::StreamInformation stream_info = {
      "test-session", "127.0.0.1", 5004, 480, 640, 25, ::mediax::rtp::ColourspaceType::kColourspaceH264Part10};
  rtp.setStreamInfo(stream_info);
  rtp.open();
  rtp.start();

  // Connect the frame signal to the slot
  connect(this, &QtTest::newFrame, &rtp, &mediax::qt6::QtRtpH264Payloader::sendFrame);
}

TEST(QtRtpTransmit, Constructors) {
  mediax::qt6::QtRtpUncompressedPayloader uncompressed_payloader;
  mediax::qt6::QtRtpH264Payloader h264_payloader;
}

TEST(QtRtpTransmit, QtTransmit) {
  QtTest test;

  // Send 10 frames 40ms appart
  for (int i = 0; i < 10; i++) {
    QTest::qWait(40);
    test.sendFrame();
  }
}

#include "qt_rtp_transmit_tests.moc"
