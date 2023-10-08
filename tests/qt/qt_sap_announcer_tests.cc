#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "qt/QtSapAnnouncer.h"
#include "qt/QtSapListener.h"

using namespace mediax::qt;
using namespace ::testing;

class QtSapAnnouncerTest : public Test {
 protected:
  void SetUp() override { qt_sap_announcer_ = std::make_unique<QtSapAnnouncer>(); }

  void TearDown() override { qt_sap_announcer_.reset(); }

  std::unique_ptr<QtSapAnnouncer> qt_sap_announcer_;
};

TEST_F(QtSapAnnouncerTest, AddSAPAnnouncement) {
  ::mediax::StreamInformation stream_information = {
      "test_session", "127.0.0.1", 5004, 480, 640, 25, ::mediax::ColourspaceType::kColourspaceRgb24, false};
  qt_sap_announcer_->deleteAllSAPAnnouncements();
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 0);
  qt_sap_announcer_->addSAPAnnouncement(stream_information);
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 1);
}

TEST_F(QtSapAnnouncerTest, DeleteAllSAPAnnouncements) {
  qt_sap_announcer_->deleteAllSAPAnnouncements();
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 0);
}

TEST_F(QtSapAnnouncerTest, Start) {
  ::mediax::StreamInformation stream_information = {
      "test_session", "127.0.0.1", 5004, 480, 640, 25, ::mediax::ColourspaceType::kColourspaceRgb24, false};
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 0);
  qt_sap_announcer_->addSAPAnnouncement(stream_information);
  qt_sap_announcer_->start();
  // Make sure we can call this again without breaking
  qt_sap_announcer_->start();
  qt_sap_announcer_->start();
  qt_sap_announcer_->start();
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
  uint16_t select = 0;
  qt_sap_announcer_->setSourceInterface(select);
}

TEST_F(QtSapAnnouncerTest, ListInterfaces) {
  uint16_t select = 0;
  qt_sap_announcer_->listInterfaces(select);
}

TEST_F(QtSapAnnouncerTest, GetActiveStreamCount) {
  ::mediax::StreamInformation stream_information = {
      "test_session", "127.0.0.1", 5004, 480, 640, 25, ::mediax::ColourspaceType::kColourspaceRgb24, false};
  qt_sap_announcer_->deleteAllSAPAnnouncements();
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 0);
  for (int i = 0; i < 1000; i++) qt_sap_announcer_->addSAPAnnouncement(stream_information);
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 1000);
  qt_sap_announcer_->deleteAllSAPAnnouncements();
  EXPECT_EQ(qt_sap_announcer_->getActiveStreamCount(), 0);
}