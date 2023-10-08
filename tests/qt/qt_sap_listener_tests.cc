#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "qt/QtSapListener.h"

using namespace mediax::qt;
using namespace ::testing;

TEST(QtSapListenerTest, Start) {
  mediax::qt::QtSapListener listener;
  listener.start();
  listener.stop();
  // Add your assertions here to test the behavior of the start() method
}

TEST(QtSapListenerTest, Stop) {
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