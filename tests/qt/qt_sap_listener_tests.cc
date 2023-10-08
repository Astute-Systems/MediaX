#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "qt/QtSapAnnouncer.h"
#include "qt/QtSapListener.h"

using namespace mediax::qt;
using namespace ::testing;

class SapReciever : public QObject {
  Q_OBJECT
 public:
  SapReciever(QObject *parent = nullptr) : QObject(parent) {}

 public slots:
  void sapDataReceived(const std::string &name, const ::mediax::sap::SDPMessage &message) {
    std::cout << "sapDataReceived SAP Name: " << name << std::endl;
  }
};

TEST(QtSapListenerTest, Start) {
  mediax::qt::QtSapListener listener;
  SapReciever reciever;
  QObject::connect(&listener, &mediax::qt::QtSapListener::sapDataReceived, &reciever, &SapReciever::sapDataReceived);

  // Transmit something
  ::mediax::StreamInformation stream_information = {
      "test_session", "127.0.0.1", 5004, 480, 640, 25, ::mediax::ColourspaceType::kColourspaceRgb24, false};
  mediax::qt::QtSapAnnouncer &announcer = mediax::qt::QtSapAnnouncer::GetInstance();
  for (int i = 0; i < 10; i++) {
    stream_information.session_name = "test_session_" + std::to_string(i);
    announcer.addSAPAnnouncement(stream_information);
  }
  announcer.start();

  listener.start();
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
