#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "qt/QtSapListener.h"

using namespace mediax::qt;
using namespace ::testing;

TEST(QtSapListenerTest, StartTest) {
  mediax::qt::QtSapListener listener;
  listener.start();
  // Add your assertions here to test the behavior of the start() method
}

TEST(QtSapListenerTest, StopTest) {
  mediax::qt::QtSapListener listener;
  listener.stop();
  // Add your assertions here to test the behavior of the stop() method
}