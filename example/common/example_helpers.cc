#include "example_helpers.h"

#include <glog/logging.h>

#include "rtp/rtp_types.h"

mediax::ColourspaceType GetMode(int mode) {
  switch (mode) {
    case 0:
      return mediax::ColourspaceType::kColourspaceRgb24;
      break;
    case 1:
      return mediax::ColourspaceType::kColourspaceYuv;
      break;
    case 2:
      return mediax::ColourspaceType::kColourspaceMono16;
      break;
    case 3:
      return mediax::ColourspaceType::kColourspaceMono8;
      break;
    case 4:
      return mediax::ColourspaceType::kColourspaceH264Part4;
      break;
    default:
      LOG(WARNING) << "Invalid video mode " << mode;
      return mediax::ColourspaceType::kColourspaceYuv;
  }
  return mediax::ColourspaceType::kColourspaceYuv;
}

std::string ModeToString(int mode) {
  switch (mode) {
    case 0:
      return "Uncompressed YUV";
    case 1:
      return "Uncompressed RGB";
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
