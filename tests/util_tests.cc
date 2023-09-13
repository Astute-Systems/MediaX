#include <png.h>
#include <stdint.h>

#include <vector>

void WritePngFile(uint8_t* rgbData, int width, int height, const char* filename) {
  FILE* fp = fopen(filename, "wb");
  if (!fp) {
    // handle error
    return;
  }

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) {
    fclose(fp);
    // handle error
    return;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    // handle error
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    // handle error
    return;
  }

  png_init_io(png_ptr, fp);

  png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png_ptr, info_ptr);

  std::vector<png_bytep> row_pointers(height);
  for (int y = 0; y < height; y++) {
    row_pointers[y] = const_cast<png_bytep>(rgbData + y * width * 3);
  }

  png_write_image(png_ptr, row_pointers.data());

  png_write_end(png_ptr, nullptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);
}