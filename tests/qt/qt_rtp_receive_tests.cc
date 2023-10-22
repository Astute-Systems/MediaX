#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <qt/QtRtpH264Depayloader.h>
#include <qt/QtRtpUncompressedDepayloader.h>



TEST(QtRtpReceive, Constructors) {
  mediax::qt::QtRtpUncompressedDepayloader uncompressed_depayloader;
  mediax::qt::QtRtpH264Depayloader h264_depayloader;
  EXPECT_EQ(uncompressed_depayloader.getState(), ::mediax::rtp::StreamState::kClosed);
  EXPECT_EQ(h264_depayloader.getState(), ::mediax::rtp::StreamState::kClosed);
}
