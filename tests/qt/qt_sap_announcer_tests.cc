#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <map>

#include "qt/QtSapAnnouncer.h"
#include "qt/QtSapListener.h"

using namespace mediax::qt;
using namespace ::testing;

class QtSapAnnouncerTest : public Test {
 protected:
  void SetUp() override {
    qt_sap_announcer_ = std::make_unique<QtSapAnnouncer>();
    qt_sap_announcer_->start();
  }

  void TearDown() override {
    qt_sap_announcer_->stop();
    qt_sap_announcer_.reset();
  }

  std::unique_ptr<QtSapAnnouncer> qt_sap_announcer_;
};

TEST_F(QtSapAnnouncerTest, AddSapAnnouncement) {
  ::mediax::rtp::StreamInformation stream_information = {
      "test_session", "127.0.0.1", 5004, 480, 640, 25, ::mediax::rtp::ColourspaceType::kColourspaceRgb24, false};

  qt_sap_announcer_->deleteAllSapAnnouncements();
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 0);
  qt_sap_announcer_->addSapAnnouncement(stream_information);
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 1);
}

TEST_F(QtSapAnnouncerTest, DeleteAllSAPAnnouncements) {
  qt_sap_announcer_->deleteAllSapAnnouncements();
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 0);
}

TEST_F(QtSapAnnouncerTest, Start) {
  ::mediax::rtp::StreamInformation stream_information = {
      "test_session", "127.0.0.1", 5004, 480, 640, 25, ::mediax::rtp::ColourspaceType::kColourspaceRgb24, false};
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 0);
  qt_sap_announcer_->addSapAnnouncement(stream_information);
  qt_sap_announcer_->start();
  qt_sap_announcer_->stop();
}

TEST_F(QtSapAnnouncerTest, Stop) {
  qt_sap_announcer_->stop();
  // Make sure we can call this again without breaking
  qt_sap_announcer_->stop();
  qt_sap_announcer_->stop();
  qt_sap_announcer_->stop();
}

TEST_F(QtSapAnnouncerTest, Restart) {
  for (int i = 0; i < 10; i++) {
    qt_sap_announcer_->start();
    qt_sap_announcer_->stop();
  }
}

TEST_F(QtSapAnnouncerTest, SetSourceInterface) {
  uint16_t select = 1;
  qt_sap_announcer_->setSourceInterface(select);
}

TEST_F(QtSapAnnouncerTest, GetInterfaces) {
  std::map<uint32_t, QString> interfaces;
  interfaces = qt_sap_announcer_->getInterfaces();
  EXPECT_GT(interfaces.size(), 0);
  // Print the interfaces
  for (auto& interface : interfaces) {
    std::cout << "Interface: " << interface.first << " " << interface.second.toStdString() << std::endl;
  }
}

TEST_F(QtSapAnnouncerTest, GetActiveStreamCount) {
  ::mediax::rtp::StreamInformation stream_information = {
      "test_session", "127.0.0.1", 5004, 480, 640, 25, ::mediax::rtp::ColourspaceType::kColourspaceRgb24, false};
  qt_sap_announcer_->deleteAllSapAnnouncements();
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 0);
  for (int i = 0; i < 1000; i++) {
    // Append i to session_name
    stream_information.session_name = stream_information.session_name + std::to_string(i);
    qt_sap_announcer_->addSapAnnouncement(stream_information);
  }
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 1000);
  qt_sap_announcer_->deleteAllSapAnnouncements();
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 0);
}

TEST_F(QtSapAnnouncerTest, GetSapAnnouncment) {
  ::mediax::rtp::StreamInformation stream_information = {
      "test_session", "127.0.0.1", 5004, 480, 640, 25, ::mediax::rtp::ColourspaceType::kColourspaceRgb24, false};

  qt_sap_announcer_->deleteAllSapAnnouncements();
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 0);
  qt_sap_announcer_->addSapAnnouncement(stream_information);
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 1);
  EXPECT_EQ(qt_sap_announcer_->getSapAnnouncement("test_session").session_name, "test_session");
  EXPECT_EQ(qt_sap_announcer_->getSapAnnouncement("test_session").hostname, "127.0.0.1");
  EXPECT_EQ(qt_sap_announcer_->getSapAnnouncement("test_session").port, 5004);
  EXPECT_EQ(qt_sap_announcer_->getSapAnnouncement("test_session").width, 640);
  EXPECT_EQ(qt_sap_announcer_->getSapAnnouncement("test_session").height, 480);
  EXPECT_EQ(qt_sap_announcer_->getSapAnnouncement("test_session").framerate, 25);
  EXPECT_EQ(qt_sap_announcer_->getSapAnnouncement("test_session").encoding,
            ::mediax::rtp::ColourspaceType::kColourspaceRgb24);
  EXPECT_EQ(qt_sap_announcer_->getSapAnnouncement("test_session").deleted, false);
}
