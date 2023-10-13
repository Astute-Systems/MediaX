#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "qt/QtSapAnnouncer.h"
#include "qt/QtSapListener.h"

using namespace mediax::qt;
using namespace ::testing;

class SapReceiver : public QObject {
  Q_OBJECT
 public:
  SapReceiver(QObject *parent = nullptr) : QObject(parent) {}

 public slots:
  void sapDataReceived(const std::string &name, const ::mediax::sap::SDPMessage &message) {
    std::cout << "sapDataReceived SAP Name: " << name << std::endl;
    std::cout << "sapDataReceived SAP Name: " << message.session_name << std::endl;
  }
};

TEST(QtSapListenerTest, DISABLED_Start) {
  mediax::qt::QtSapListener listener;
  SapReceiver receiver;
  QObject::connect(&listener, &mediax::qt::QtSapListener::sapData, &receiver, &SapReceiver::sapDataReceived);

  // Transmit something
  ::mediax::StreamInformation stream_information = {
      "test_session", "127.0.0.1", 5004, 480, 640, 25, ::mediax::ColourspaceType::kColourspaceRgb24, false};
  mediax::qt::QtSapAnnouncer &announcer = mediax::qt::QtSapAnnouncer::getInstance();
  for (int i = 0; i < 10; i++) {
    stream_information.session_name = "test_session_" + std::to_string(i);
    announcer.AddSapAnnouncement(stream_information);
  }
  announcer.start();

  listener.start();

  mediax::sap::SDPMessage sdp_message = {
      1, "test_dummy_session", "192.168.1.1", "127.0.0.1", 5004, 480, 640, "RGB", 25, 24, false, "test"};
  emit listener.sapData("this_is_not_a_real_message", sdp_message);

  // Sleep for two seconds
  sleep(2);
  listener.stop();

  announcer.stop();
}

TEST(QtSapListenerTest, DISABLED_Stop) {
  mediax::qt::QtSapListener listener;
  listener.start();
  listener.start();
  listener.start();
  listener.start();
  listener.stop();
  listener.stop();
  listener.stop();
  listener.stop();
  // Add your assertions here to test the behavior of the stop() method
}

#include "qt_sap_listener_tests.moc"
