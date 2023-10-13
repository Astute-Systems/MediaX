#include "example_helpers.h"

#include <glog/logging.h>

#include "rtp/rtp_types.h"

mediax::rtp::ColourspaceType GetMode(int mode) {
  switch (mode) {
    case 0:
      return mediax::rtp::ColourspaceType::kColourspaceRgb24;
      break;
    case 1:
      return mediax::rtp::ColourspaceType::kColourspaceYuv;
      break;
    case 2:
      return mediax::rtp::ColourspaceType::kColourspaceMono16;
      break;
    case 3:
      return mediax::rtp::ColourspaceType::kColourspaceMono8;
      break;
    case 4:
      return mediax::rtp::ColourspaceType::kColourspaceH264Part4;
      break;
    default:
      LOG(WARNING) << "Invalid video mode " << mode;
      return mediax::rtp::ColourspaceType::kColourspaceYuv;
  }
  return mediax::rtp::ColourspaceType::kColourspaceYuv;
}

std::string ModeToString(int mode) {
  switch (mode) {
    case 0:
      return "Uncompressed RGB";
    case 1:
      return "Uncompressed YUV";
    case 2:
      return "Mono16";
    case 3:
      return "Mono8";
    case 4:
      return "H.264";
    default:
      break;
  }
  LOG(WARNING) << "Invalid video mode" << mode;
  return "Unknown";
}
