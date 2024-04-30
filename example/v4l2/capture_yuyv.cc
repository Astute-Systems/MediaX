#include <assert.h>
#include <capture_yuyv.h>
#include <errno.h>
#include <fcntl.h>   // low-level i/o
#include <getopt.h>  // getopt_long()
#include <linux/videodev2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define WIDTH 640
#define HEIGHT 480
#define GRAB_NUM_FRAMES 100
#define BYTESPERPIXEL 3  // for color

typedef enum {
  IO_METHOD_READ,
  IO_METHOD_MMAP,
  IO_METHOD_USERPTR,
} io_method;

struct buffer {
  void *start;
  size_t length;
};

static char dev_name[256];
static io_method io = IO_METHOD_MMAP;
static int fd = -1;
struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;

// This YUV422toBGR888() is ported from v4l2grab.c by Tobias Muller under GNU-GPL
//  Convert from YUV422 format to RGB888. Formulae are described on http://en.wikipedia.org/wiki/YUV/
//
//    \param width width of image
//    \param height height of image
//    \param src source
//    \param dst destination

static void YUV422toBGR888(int width, int height, unsigned char *src, unsigned char *dst) {
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

static void errno_exit(const char *s) {
  fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));

  exit(EXIT_FAILURE);
}

static int xioctl(int fd, int request, void *arg) {
  int r;

  do r = ioctl(fd, request, arg);
  while (-1 == r && EINTR == errno);

  return r;
}

static void process_image(const void *p, int frame) {
  image_info_t info;
  char szFilebase[60] = "out_";
  char szFilename[60];

  // fputc ('.', stdout);
  // fflush (stdout);

  // printf ("frame = %i\tptr = %p\n", frame, p);
  printf("frame = %i,\t", frame);

  // set up the image save( or if SDL, display to screen)
  info.width = WIDTH;
  info.height = HEIGHT;
  info.stride = info.width * BYTESPERPIXEL;
  sprintf(szFilename, "%s%d", szFilebase, frame);

  save_image_uncompressed((const unsigned char *)p, szFilename, &info, (BYTESPERPIXEL == 3) ? 1 : 2);

  printf("writing into file %s.p%cm\n", szFilename, (BYTESPERPIXEL == 3) ? 'p' : 'g');
}

static int read_frame(int count) {
  struct v4l2_buffer buf;
  unsigned int i;

  switch (io) {
    case IO_METHOD_READ:
      if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
        switch (errno) {
          case EAGAIN:
            return 0;

          case EIO:
            /* Could ignore EIO, see spec. */

            /* fall through */

          default:
            errno_exit("read");
        }
      }

      process_image(buffers[0].start, count);

      break;

    case IO_METHOD_MMAP:
      CLEAR(buf);

      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;

      if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
          case EAGAIN:
            return 0;

          case EIO:
            /* Could ignore EIO, see spec. */

            /* fall through */

          default:
            errno_exit("VIDIOC_DQBUF");
        }
      }

      assert(buf.index < n_buffers);

      process_image(buffers[buf.index].start, count);

      if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) errno_exit("VIDIOC_QBUF");

      break;

    case IO_METHOD_USERPTR:
      CLEAR(buf);

      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_USERPTR;

      if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
          case EAGAIN:
            return 0;

          case EIO:
            /* Could ignore EIO, see spec. */

            /* fall through */

          default:
            errno_exit("VIDIOC_DQBUF");
        }
      }

      for (i = 0; i < n_buffers; ++i)
        if (buf.m.userptr == (unsigned long)buffers[i].start && buf.length == buffers[i].length) break;

      assert(i < n_buffers);

      process_image((void *)buf.m.userptr, count);

      if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) errno_exit("VIDIOC_QBUF");

      break;
  }

  return 1;
}

void V4l2_Capture_YUYV::mainloop(void) {
  unsigned int count;

  count = GRAB_NUM_FRAMES;

  while (count-- > 0) {
    for (;;) {
      fd_set fds;
      struct timeval tv;
      int r;

      FD_ZERO(&fds);
      FD_SET(fd, &fds);

      /* Timeout. */
      tv.tv_sec = 2;
      tv.tv_usec = 0;

      r = select(fd + 1, &fds, NULL, NULL, &tv);

      if (-1 == r) {
        if (EINTR == errno) continue;

        errno_exit("select");
      }

      if (0 == r) {
        fprintf(stderr, "select timeout\n");
        exit(EXIT_FAILURE);
      }

      if (read_frame(GRAB_NUM_FRAMES - count)) break;

      /* EAGAIN - continue select loop. */
    }
  }
}

void V4l2_Capture_YUYV::stop_capturing(void) {
  enum v4l2_buf_type type;

  switch (io) {
    case IO_METHOD_READ:
      /* Nothing to do. */
      break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

      if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)) errno_exit("VIDIOC_STREAMOFF");

      break;
  }
}

void V4l2_Capture_YUYV::start_capturing(void) {
  unsigned int i;
  enum v4l2_buf_type type;

  switch (io) {
    case IO_METHOD_READ:
      /* Nothing to do. */
      break;

    case IO_METHOD_MMAP:
      for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) errno_exit("VIDIOC_QBUF");
      }

      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

      if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)) errno_exit("VIDIOC_STREAMON");

      break;

    case IO_METHOD_USERPTR:
      for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;
        buf.m.userptr = (unsigned long)buffers[i].start;
        buf.length = buffers[i].length;

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) errno_exit("VIDIOC_QBUF");
      }

      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

      if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)) errno_exit("VIDIOC_STREAMON");

      break;
  }
}

void V4l2_Capture_YUYV::uninit_device(void) {
  unsigned int i;

  switch (io) {
    case IO_METHOD_READ:
      free(buffers[0].start);
      break;

    case IO_METHOD_MMAP:
      for (i = 0; i < n_buffers; ++i)
        if (-1 == munmap(buffers[i].start, buffers[i].length)) errno_exit("munmap");
      break;

    case IO_METHOD_USERPTR:
      for (i = 0; i < n_buffers; ++i) free(buffers[i].start);
      break;
  }

  free(buffers);
}

static void init_read(unsigned int buffer_size) {
  buffers = (buffer *)calloc(1, sizeof(*buffers));

  if (!buffers) {
    fprintf(stderr, "Out of memory\n");
    exit(EXIT_FAILURE);
  }

  buffers[0].length = buffer_size;
  buffers[0].start = malloc(buffer_size);

  if (!buffers[0].start) {
    fprintf(stderr, "Out of memory\n");
    exit(EXIT_FAILURE);
  }
}

void V4l2_Capture_YUYV::init_mmap(void) {
  struct v4l2_requestbuffers req;

  CLEAR(req);

  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
    if (EINVAL == errno) {
      fprintf(stderr,
              "%s does not support "
              "memory mapping\n",
              dev_name);
      exit(EXIT_FAILURE);
    } else {
      errno_exit("VIDIOC_REQBUFS");
    }
  }

  if (req.count < 2) {
    fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
    exit(EXIT_FAILURE);
  }

  buffers = (buffer *)calloc(req.count, sizeof(*buffers));

  if (!buffers) {
    fprintf(stderr, "Out of memory\n");
    exit(EXIT_FAILURE);
  }

  for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
    struct v4l2_buffer buf;

    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = n_buffers;

    if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)) errno_exit("VIDIOC_QUERYBUF");

    buffers[n_buffers].length = buf.length;
    buffers[n_buffers].start = mmap(NULL /* start anywhere */, buf.length, PROT_READ | PROT_WRITE /* required */,
                                    MAP_SHARED /* recommended */, fd, buf.m.offset);

    if (MAP_FAILED == buffers[n_buffers].start) errno_exit("mmap");
  }
}

void V4l2_Capture_YUYV::init_userp(unsigned int buffer_size) {
  struct v4l2_requestbuffers req;

  CLEAR(req);

  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_USERPTR;

  if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
    if (EINVAL == errno) {
      fprintf(stderr,
              "%s does not support "
              "user pointer i/o\n",
              dev_name);
      exit(EXIT_FAILURE);
    } else {
      errno_exit("VIDIOC_REQBUFS");
    }
  }

  buffers = (buffer *)calloc(4, sizeof(*buffers));

  if (!buffers) {
    fprintf(stderr, "Out of memory\n");
    exit(EXIT_FAILURE);
  }

  for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
    buffers[n_buffers].length = buffer_size;
    buffers[n_buffers].start = malloc(buffer_size);

    if (!buffers[n_buffers].start) {
      fprintf(stderr, "Out of memory\n");
      exit(EXIT_FAILURE);
    }
  }
}

void V4l2_Capture_YUYV::init_device(void) {
  struct v4l2_capability cap;
  struct v4l2_cropcap cropcap;
  struct v4l2_crop crop;
  struct v4l2_format fmt;
  unsigned int min;
  int input, standard;

  printf("\nstaring device initialization, for %s, ...\n", dev_name);

  if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
    if (EINVAL == errno) {
      fprintf(stderr, "%s is no V4L2 device\n", dev_name);
      exit(EXIT_FAILURE);
    } else {
      errno_exit("VIDIOC_QUERYCAP");
    }
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    fprintf(stderr, "%s is no video capture device\n", dev_name);
    exit(EXIT_FAILURE);
  }

  switch (io) {
    case IO_METHOD_READ:
      if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
        fprintf(stderr, "%s does not support read i/o\n", dev_name);
        exit(EXIT_FAILURE);
      }

      break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
      if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "%s does not support streaming i/o\n", dev_name);
        exit(EXIT_FAILURE);
      }

      break;
  }

  /* Select video input, video standard and tune here. */

  /* Reset Cropping */
  printf("...reset cropping of %s ...\n", dev_name);
  CLEAR(cropcap);
  cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (-1 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
    /* Errors ignored. */
  }

  crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  crop.c = cropcap.defrect; /* reset to default */

  if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
    switch (errno) {
      case EINVAL:
        /* Cropping not supported. */
        break;
      default:
        /* Errors ignored. */
        break;
    }
  }
  sleep(1);

  /* Select standard */
  printf("...select standard of %s ...\n", dev_name);
  // standard = V4L2_STD_NTSC;
  standard = V4L2_STD_PAL;
  if (-1 == xioctl(fd, VIDIOC_S_STD, &standard)) {
    perror("VIDIOC_S_STD");
    exit(EXIT_FAILURE);
  }
  sleep(1);

  /* Select input */
  printf("...select input channel of %s ...\n\n", dev_name);
  input = 0;  // Composite-0
  if (-1 == ioctl(fd, VIDIOC_S_INPUT, &input)) {
    perror("VIDIOC_S_INPUT");
    exit(EXIT_FAILURE);
  }
  sleep(1);

  CLEAR(fmt);

  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = WIDTH;
  fmt.fmt.pix.height = HEIGHT;
  // fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  // fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
  if (BYTESPERPIXEL == 3)
    // fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  else
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
  fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
  // fmt.fmt.pix.field       = V4L2_FIELD_ANY;     //works too

  if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) errno_exit("VIDIOC_S_FMT");

  /* Note VIDIOC_S_FMT may change width and height. */

  /* Buggy driver paranoia. */
  min = fmt.fmt.pix.width * BYTESPERPIXEL;
  if (fmt.fmt.pix.bytesperline < min) fmt.fmt.pix.bytesperline = min;
  min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
  if (fmt.fmt.pix.sizeimage < min) fmt.fmt.pix.sizeimage = min;

  switch (io) {
    case IO_METHOD_READ:
      init_read(fmt.fmt.pix.sizeimage);
      break;

    case IO_METHOD_MMAP:
      init_mmap();
      break;

    case IO_METHOD_USERPTR:
      init_userp(fmt.fmt.pix.sizeimage);
      break;
  }
}

void V4l2_Capture_YUYV::close_device(void) {
  if (-1 == close(fd)) errno_exit("close");

  fd = -1;
}

void V4l2_Capture_YUYV::open_device(void) {
  struct stat st;

  if (-1 == stat(dev_name, &st)) {
    fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno));
    exit(EXIT_FAILURE);
  }

  if (!S_ISCHR(st.st_mode)) {
    fprintf(stderr, "%s is no device\n", dev_name);
    exit(EXIT_FAILURE);
  }

  fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

  if (-1 == fd) {
    fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/** saves an image to bmp file
 *  @param filename is the full file path to save( minus the extension)
 *  @param *image, pointer to image data in Y Cr Cb format
 *  @param type is the type( 0-- BMP,  1--ppm,  2--pgm)
 *  @return 0 on success, -1 if file invalid
 */
int save_image_uncompressed(const unsigned char *image, const char *szFilename, image_info_t *info, int type) {
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

static void usage(FILE *fp, int argc, char **argv) {
  fprintf(fp,
          "Usage: %s [options]\n\n"
          "Options:\n"
          "-d | --device name   Video device name [/dev/video]\n"
          "-h | --help          Print this message\n"
          "-m | --mmap          Use memory mapped buffers\n"
          "-r | --read          Use read() calls\n"
          "-u | --userp         Use application allocated buffers\n"
          "\n",
          argv[0]);
}

static const char short_options[] = "d:hmru";

static const struct option long_options[] = {{"device", required_argument, NULL, 'd'}, {"help", no_argument, NULL, 'h'},
                                             {"mmap", no_argument, NULL, 'm'},         {"read", no_argument, NULL, 'r'},
                                             {"userp", no_argument, NULL, 'u'},        {0, 0, 0, 0}};

int main(int argc, char **argv) {
  V4l2_Capture_YUYV capture;

  strncpy(dev_name, "/dev/video0", sizeof(dev_name));
  // dev_name = "/dev/video0";	// Device-#1
  // dev_name = "/dev/video1";	// Device-#2
  // dev_name = "/dev/video2";	// Device-#3
  // dev_name = "/dev/video3";	// Device-#4
  // dev_name = "/dev/video4";	// Device-#5
  // dev_name = "/dev/video5";	// Device-#6
  // dev_name = "/dev/video6";	// Device-#7
  // dev_name = "/dev/video7";	// Device-#8
  //  Device menetioned above implies:
  //		Board for 614/314/611/311, or
  //       Channel for 811/911:
  //			(#1~4 for 1st Board; #5~8 for 2nd Board)

  for (;;) {
    int index;
    int c;

    c = getopt_long(argc, argv, short_options, long_options, &index);

    if (-1 == c) break;

    switch (c) {
      case 0: /* getopt_long() flag */
        // break;
        {
          usage(stderr, argc, argv);
          exit(EXIT_FAILURE);
        }

      case 'd':
        // dev_name = optarg;
        // strncpy(dev_name, optarg, sizeof(dev_name));
        break;

      case 'h':
        usage(stdout, argc, argv);
        exit(EXIT_SUCCESS);

      case 'm':
        io = IO_METHOD_MMAP;
        break;

      case 'r':
        io = IO_METHOD_READ;
        break;

      case 'u':
        io = IO_METHOD_USERPTR;
        break;

      default:
        usage(stderr, argc, argv);
        exit(EXIT_FAILURE);
    }
  }

  capture.open_device();

  capture.init_device();

  capture.start_capturing();

  capture.mainloop();

  capture.stop_capturing();

  capture.uninit_device();

  capture.close_device();

  exit(EXIT_SUCCESS);

  return 0;
}
