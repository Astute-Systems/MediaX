#include "utils.h"

// This YUV422toBGR888() is ported from v4l2grab.c by Tobias Muller under GNU-GPL
//  Convert from YUV422 format to RGB888. Formulae are described on http://en.wikipedia.org/wiki/YUV/
//
//    \param width width of image
//    \param height height of image
//    \param src source
//    \param dst destination

void YUV422toBGR888(int width, int height, unsigned char *src, unsigned char *dst) {
  int line, column;
  unsigned char *py, *pu, *pv;
  unsigned char *tmp = dst;

  // In this format each four bytes is two pixels. Each four bytes is two Y's, a Cb and a Cr.
  // Each Y goes to one of the pixels, and the Cb and Cr belong to both pixels.
  py = src;
  pu = src + 1;
  pv = src + 3;

#define CLIP(x) ((x) >= 0xFF ? 0xFF : ((x) <= 0x00 ? 0x00 : (x)))

  for (line = 0; line < height; ++line) {
    for (column = 0; column < width; ++column) {
      *tmp++ = CLIP((double)*py + 1.772 * ((double)*pu - 128.0));                                  // B
      *tmp++ = CLIP((double)*py - 0.344 * ((double)*pu - 128.0) - 0.714 * ((double)*pv - 128.0));  // G
      *tmp++ = CLIP((double)*py + 1.402 * ((double)*pv - 128.0));                                  // R
      // increase py every time
      py += 2;
      // increase pu, pv every time
      if ((column & 1) == 1) {
        pu += 4;
        pv += 4;
      }
    }
  }
}

// saves an image to bmp file
//  @param filename is the full file path to save( minus the extension)
//  @param *image, pointer to image data in Y Cr Cb format
//  @param type is the type( 0-- BMP,  1--ppm,  2--pgm)
//  @return 0 on success, -1 if file invalid
//
int SaveImageUncompressed(const unsigned char *image, const char *szFilename, image_info_t *info, int type) {
  char ppmheader[256];
  char name[60];
  FILE *fptr;
  int val;
  int rlen;  // row length or number of columns( unsigned chars== pixels * unsigned chars per pixel)
  int clen;  // column length or number of rows
  int bpp;   // unsigned chars per pixel! not bits
  int i, j;
  unsigned char *src = (unsigned char *)image;
  unsigned char *dst = (unsigned char *)malloc(info->width * info->height * 3 * sizeof(char));

  rlen = info->stride;
  clen = info->height;
  bpp = info->stride / info->width;

  // open the file
  if (type == 1)
    sprintf(name, "%s.%s", szFilename, "ppm");
  else if (type == 2)
    sprintf(name, "%s.%s", szFilename, "pgm");
  else
    sprintf(name, "%s.%s", szFilename, "bmp");

  fptr = fopen(name, "wb");

  if (fptr == NULL) return -1;

  if (type == 1)  // for ppm
  {
    // sprintf( ppmheader, "P6\n%d %d\n255\n", info->width, info->height);
    sprintf(ppmheader, "P6\n#ppm image\n%d %d\n255\n", info->width, info->height);
    fwrite(ppmheader, 1, strlen(ppmheader), fptr);

    // convert from YUV422 to RGB888
    YUV422toBGR888(info->width, info->height, src, dst);

    // write out to the output file
    for (i = 0; i < info->height; i++) {
      // fwrite( &image[info->stride*i], 1, info->stride, fptr);
      for (j = 0; j < info->width; j++) {
        fwrite(&dst[info->stride * i + j * 3 + 2], 1, 1, fptr);
        fwrite(&dst[info->stride * i + j * 3 + 1], 1, 1, fptr);
        fwrite(&dst[info->stride * i + j * 3 + 0], 1, 1, fptr);
      }
    }
  } else if (type == 2)  // for pgm
  {
    // sprintf( ppmheader, "P5\n%d %d\n255\n", info->width, info->height);
    sprintf(ppmheader, "P5\n#ppm image\n%d %d\n255\n", info->width, info->height);
    fwrite(ppmheader, 1, strlen(ppmheader), fptr);
    // write out the rows
    for (i = 0; i < info->height; i++) {
      // fwrite( &image[info->stride*i], 1, info->stride, fptr);
      for (j = 0; j < info->width; j++) {
        fwrite(&image[info->stride * i + j], 1, 1, fptr);
      }
    }
  } else  // for BMP. needs to be debugged/tested
  {
    val = 0x4d42;
    fwrite(&val, 1, 2, fptr);  // bmp signature
    val = (rlen * clen / bpp) + 54;
    fwrite(&val, 1, 4, fptr);  // size of bmp file
    val = 0;
    fwrite(&val, 1, 2, fptr);  // must be 0
    val = 0;
    fwrite(&val, 1, 2, fptr);  // must be 0
    val = 54;
    fwrite(&val, 1, 4, fptr);  // offset to image start
    val = 40;
    fwrite(&val, 1, 4, fptr);  // must be 40
    val = rlen / bpp;
    fwrite(&val, 1, 4, fptr);  // image width in pixels
    val = clen;
    fwrite(&val, 1, 4, fptr);  // image height in pixels
    val = 1;
    fwrite(&val, 1, 2, fptr);  // must be 1
    val = bpp * 8;
    fwrite(&val, 1, 2, fptr);  // bits per pixel
    val = 0;
    fwrite(&val, 1, 4, fptr);  // compression (none)
    val = (rlen * clen / bpp);
    fwrite(&val, 1, 4, fptr);  // size of image data
    val = 2835;
    fwrite(&val, 1, 4, fptr);  // horizontal pixels/m
    val = 2835;
    fwrite(&val, 1, 4, fptr);  // vertical pixels/m
    val = 0;
    fwrite(&val, 1, 4, fptr);  // colors in image, or 0
    val = 0;
    fwrite(&val, 1, 4, fptr);  // important colors,or 0

#ifndef REVERSE_BMP
    // BMP reverse the image order -- retrieves in reverse order
    for (i = clen - 1; i >= 0; i--)  // rows
    {
      fwrite(&image[i * rlen], 1, rlen, fptr);
    }
#else
    // regular BMP order.
    for (i = 0; i < clen; i++)  // rows
    {
      fwrite(&image[i * rlen], 1, rlen, fptr);
    }
#endif
  }

  fclose(fptr);
  return 0;
}