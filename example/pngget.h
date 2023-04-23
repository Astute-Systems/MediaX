//
// Copyright (C) 2023 DefenceX PTY LTD. All rights reserved.
//
// This software is distributed under the included copyright license.
// Any redistribution or reproduction, in part or in whole, in any form or medium, is strictly prohibited without the
// prior written consent of DefenceX PTY LTD.
//
// For any inquiries or concerns, please contact:
// DefenceX PTY LTD
// Email: enquiries@defencex.ai
//
///
/// \file pngget.h

#ifndef PNGGET_H
#define PNGGET_H

#include <png.h>
#include <stdint.h>

#include <string>
#include <vector>

class Png {
 public:
  png_bytep* GetRowPointwes();
  void ReadPngFile(char* file_name);
  std::vector<uint8_t> ReadPngRgb24(std::string_view filename) const;

 private:
  png_structp png_ptr;

  png_bytep* row_pointers;

  int x;
  int y;

  int width;
  int height;
  png_byte color_type;
  png_byte bit_depth;

  png_infop info_ptr;
  int number_of_passes;

  void Abort(const char* s, ...);
};
#endif
