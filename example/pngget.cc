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
/// \file pngget.cc
///

#include "pngget.h"

#include <png.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

std::vector<uint8_t> read_png_rgb24(std::string_view filename) {
  FILE* fp = fopen(std::string(filename).c_str(), "rb");
  if (!fp) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return {};
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) {
    std::cerr << "Error creating read struct" << std::endl;
    fclose(fp);
    return {};
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    std::cerr << "Error creating info struct" << std::endl;
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    fclose(fp);
    return {};
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    std::cerr << "Error during PNG read" << std::endl;
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
    return {};
  }

  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);

  png_uint_32 width;
  png_uint_32 height;
  int bit_depth;
  int color_type;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, nullptr, nullptr, nullptr);

  png_set_expand(png_ptr);
  png_set_strip_16(png_ptr);
  png_set_gray_to_rgb(png_ptr);
  png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
  png_read_update_info(png_ptr, info_ptr);

  std::vector<png_bytep> row_pointers(height);
  std::vector<uint8_t> rgb24_buffer((width * height) * 4);

  for (png_uint_32 y = 0; y < height; y++) {
    row_pointers[y] = reinterpret_cast<png_bytep>(&rgb24_buffer[y * width * 4]);
  }

  png_read_image(png_ptr, row_pointers.data());

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  fclose(fp);

  return rgb24_buffer;
}

png_structp png_ptr;

png_bytep* row_pointers;
png_bytep* get_row_pointwes() { return row_pointers; }

void abort_(const char* s, ...) {
  va_list args;
  va_start(args, s);
  vfprintf(stderr, s, args);
  fprintf(stderr, "\n");
  va_end(args);
  abort();
}

int x;
int y;

int width;
int height;
png_byte color_type;
png_byte bit_depth;

png_infop info_ptr;
int number_of_passes;

void read_png_file(char* file_name) {
  char header[8];  // 8 is the maximum size that can be checked

  // open file and test for it being a png
  FILE* fp = fopen(file_name, "rb");
  if (!fp) abort_("[read_png_file] File %s could not be opened for reading", file_name);
  if (fread(header, 1, 8, fp)) abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);
  if (png_sig_cmp((png_bytep)header, 0, 8))
    abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);

  // initialize stuff
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png_ptr) abort_("[read_png_file] png_create_read_struct failed");

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) abort_("[read_png_file] png_create_info_struct failed");

  if (setjmp(png_jmpbuf(png_ptr))) abort_("[read_png_file] Error during init_io");

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);

  width = png_get_image_width(png_ptr, info_ptr);
  height = png_get_image_height(png_ptr, info_ptr);

  color_type = png_get_color_type(png_ptr, info_ptr);
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);

  number_of_passes = png_set_interlace_handling(png_ptr);
  png_read_update_info(png_ptr, info_ptr);

  // read file
  if (setjmp(png_jmpbuf(png_ptr))) abort_("[read_png_file] Error during read_image");

  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * width);
  for (y = 0; y < height; y++) row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));

  png_read_image(png_ptr, row_pointers);

  fclose(fp);
}
