#ifndef PNGGET_H
#define PNGGET_H

#include <png.h>
#include <stdint.h>

#include <vector>
png_bytep* get_row_pointwes();
void read_png_file(char* file_name);
std::vector<uint8_t> read_png_rgb24(const char* filename);
#endif
