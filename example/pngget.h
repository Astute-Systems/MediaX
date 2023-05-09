//
// Copyright (c) 2023, DefenceX PTY LTD
//
// This file is part of the VivoeX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
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
