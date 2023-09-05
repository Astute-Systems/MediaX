///  \file main.cc

#include <gflags/gflags.h>  // for ParseCommandLineFlags
#include <glog/logging.h>
#include <gtest/gtest.h>  // for InitGoogleTest, RUN_ALL_TESTS

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  return RUN_ALL_TESTS();
}