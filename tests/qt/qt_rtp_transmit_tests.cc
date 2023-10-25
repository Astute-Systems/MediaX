#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <qt/QtRtpH264Payloader.h>
#include <qt/QtRtpUncompressedPayloader.h>
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
  void newFrame(QByteArray *frame);

 public:
  void sendFrame() {
    // Create a frame of RGB pixels in QByteArray
    QByteArray frame;
    frame.resize(640 * 480 * 3);
    frame.fill(0);
    // Send the frame to the payloader
    emit newFrame(&frame);
  }

 private:
  mediax::qt::QtRtpH264Payloader rtp;
};

QtTest::QtTest() {
  rtp.setStreamInfo("127.0.0.1", 5004, "qt-test", 480, 640, 25);
  rtp.open();
  rtp.start();

  // Connect the frame signal to the slot
  connect(this, &QtTest::newFrame, &rtp, &mediax::qt::QtRtpH264Payloader::sendFrame);
}

TEST(QtRtpTransmit, Constructors) {
  mediax::qt::QtRtpUncompressedPayloader uncompressed_payloader;
  mediax::qt::QtRtpH264Payloader h264_payloader;
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