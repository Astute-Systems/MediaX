///  \file main.cc

#include <gflags/gflags.h>  // for ParseCommandLineFlags
#include <gtest/gtest.h>    // for InitGoogleTest, RUN_ALL_TESTS
#include <rtp/rtp.h>

#include <QCoreApplication>
#include <QTimer>

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  mediax::InitRtp(argc, argv);
  int ret = RUN_ALL_TESTS();
  mediax::RtpCleanup();
  return ret;
}