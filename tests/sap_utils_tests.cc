#include <gtest/gtest.h>

#include "sap/sap_utils.h"

TEST(SapUtilsTest, GetSdpColourspaceTest) {
  EXPECT_EQ(mediax::sap::GetSdpColourspace(mediax::ColourspaceType::kColourspaceRgb24), "RGB");
  EXPECT_EQ(mediax::sap::GetSdpColourspace(mediax::ColourspaceType::kColourspaceYuv), "YCbCr-4:2:2");
  EXPECT_EQ(mediax::sap::GetSdpColourspace(mediax::ColourspaceType::kColourspaceMono16), "Mono");
  EXPECT_EQ(mediax::sap::GetSdpColourspace(mediax::ColourspaceType::kColourspaceMono8), "Mono");
  EXPECT_EQ(mediax::sap::GetSdpColourspace(static_cast<mediax::ColourspaceType>(-1)), "unknown");
}