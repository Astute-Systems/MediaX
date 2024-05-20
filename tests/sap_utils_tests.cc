#include <gtest/gtest.h>

#include "sap/sap_utils.h"

TEST(SapUtilsTest, GetSdpColourspaceTest) {
  EXPECT_EQ(mediax::sap::GetSdpColourspace(mediax::rtp::ColourspaceType::kColourspaceRgb24), "RGB");
  EXPECT_EQ(mediax::sap::GetSdpColourspace(mediax::rtp::ColourspaceType::kColourspaceYuv422), "YCbCr-4:2:2");
  EXPECT_EQ(mediax::sap::GetSdpColourspace(mediax::rtp::ColourspaceType::kColourspaceMono16), "Mono");
  EXPECT_EQ(mediax::sap::GetSdpColourspace(mediax::rtp::ColourspaceType::kColourspaceMono8), "Mono");
  EXPECT_EQ(mediax::sap::GetSdpColourspace(static_cast<mediax::rtp::ColourspaceType>(-1)), "unknown");
}