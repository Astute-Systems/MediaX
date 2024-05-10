//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the MediaX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief A simple video receiver example
///
/// \file capture_yuyv.cc
///

#include <assert.h>
#include <capture.h>
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
#include <utils.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define WIDTH 640
#define HEIGHT 480
#define GRAB_NUM_FRAMES 100
#define BYTESPERPIXEL 3  // for color

struct buffer {
  void *start;
  size_t length;
};

static int fd = -1;
struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;

V4l2_Capture_YUYV::V4l2_Capture_YUYV(std::string dev_name, IoMethod io) : dev_name_(dev_name), io_(io) {}

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

  SaveImageUncompressed((const unsigned char *)p, szFilename, &info, (BYTESPERPIXEL == 3) ? 1 : 2);

  printf("writing into file %s.p%cm\n", szFilename, (BYTESPERPIXEL == 3) ? 'p' : 'g');
}

int V4l2_Capture_YUYV::read_frame(int count) {
  struct v4l2_buffer buf;
  unsigned int i;

  switch (io_) {
    case IoMethod::IO_METHOD_READ:
      if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
        switch (errno) {
          case EAGAIN:
            return 0;

          case EIO:
            // Could ignore EIO, see spec. */

            // fall through */

          default:
            errno_exit("read");
        }
      }

      process_image(buffers[0].start, count);

      break;

    case IoMethod::IO_METHOD_MMAP:
      CLEAR(buf);

      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;

      if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
          case EAGAIN:
            return 0;

          case EIO:
            // Could ignore EIO, see spec. */

            // fall through */

          default:
            errno_exit("VIDIOC_DQBUF");
        }
      }

      assert(buf.index < n_buffers);

      process_image(buffers[buf.index].start, count);

      if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) errno_exit("VIDIOC_QBUF");

      break;

    case IoMethod::IO_METHOD_USERPTR:
      CLEAR(buf);

      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_USERPTR;

      if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
          case EAGAIN:
            return 0;

          case EIO:
            // Could ignore EIO, see spec. */

            // fall through */

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

void V4l2_Capture_YUYV::Run(void) {
  unsigned int count;

  count = GRAB_NUM_FRAMES;

  while (count-- > 0) {
    for (;;) {
      fd_set fds;
      struct timeval tv;
      int r;

      FD_ZERO(&fds);
      FD_SET(fd, &fds);

      // Timeout. */
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

      // EAGAIN - continue select loop.
    }
  }
}

void V4l2_Capture_YUYV::StopDevice(void) {
  enum v4l2_buf_type type;

  switch (io_) {
    case IoMethod::IO_METHOD_READ:
      // Nothing to do. */
      break;

    case IoMethod::IO_METHOD_MMAP:
    case IoMethod::IO_METHOD_USERPTR:
      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

      if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)) errno_exit("VIDIOC_STREAMOFF");

      break;
  }
}

void V4l2_Capture_YUYV::StartDevice(void) {
  unsigned int i;
  enum v4l2_buf_type type;

  switch (io_) {
    case IoMethod::IO_METHOD_READ:
      // Nothing to do. */
      break;

    case IoMethod::IO_METHOD_MMAP:
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

    case IoMethod::IO_METHOD_USERPTR:
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

void V4l2_Capture_YUYV::UninitaliseDevice(void) {
  unsigned int i;

  switch (io_) {
    case IoMethod::IO_METHOD_READ:
      free(buffers[0].start);
      break;

    case IoMethod::IO_METHOD_MMAP:
      for (i = 0; i < n_buffers; ++i)
        if (-1 == munmap(buffers[i].start, buffers[i].length)) errno_exit("munmap");
      break;

    case IoMethod::IO_METHOD_USERPTR:
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

void V4l2_Capture_YUYV::InitaliseMmap(void) {
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
              dev_name_.c_str());
      exit(EXIT_FAILURE);
    } else {
      errno_exit("VIDIOC_REQBUFS");
    }
  }

  if (req.count < 2) {
    fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name_.c_str());
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
    buffers[n_buffers].start = mmap(NULL,  // start anywhere
                                    buf.length,
                                    PROT_READ | PROT_WRITE,  // required
                                    MAP_SHARED,              // recommended

                                    fd, buf.m.offset);

    if (MAP_FAILED == buffers[n_buffers].start) errno_exit("mmap");
  }
}

void V4l2_Capture_YUYV::InitaliseUserp(unsigned int buffer_size) {
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
              dev_name_.c_str());
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

void V4l2_Capture_YUYV::InitaliseDevice(void) {
  struct v4l2_capability cap;
  struct v4l2_cropcap cropcap;
  struct v4l2_crop crop;
  struct v4l2_format fmt;
  unsigned int min;
  int input, standard;

  printf("\nstaring device initialization, for %s, ...\n", dev_name_.c_str());

  if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
    if (EINVAL == errno) {
      fprintf(stderr, "%s is no V4L2 device\n", dev_name_.c_str());
      exit(EXIT_FAILURE);
    } else {
      errno_exit("VIDIOC_QUERYCAP");
    }
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    fprintf(stderr, "%s is no video capture device\n", dev_name_.c_str());
    exit(EXIT_FAILURE);
  }

  switch (io_) {
    case IoMethod::IO_METHOD_READ:
      if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
        fprintf(stderr, "%s does not support read i/o\n", dev_name_.c_str());
        exit(EXIT_FAILURE);
      }

      break;

    case IoMethod::IO_METHOD_MMAP:
    case IoMethod::IO_METHOD_USERPTR:
      if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "%s does not support streaming i/o\n", dev_name_.c_str());
        exit(EXIT_FAILURE);
      }

      break;
  }

  // Select video input, video standard and tune here. */

  // Reset Cropping */
  printf("...reset cropping of %s ...\n", dev_name_.c_str());
  CLEAR(cropcap);
  cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (-1 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
    // Errors ignored. */
  }

  crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  crop.c = cropcap.defrect;  // reset to default */

  if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
    switch (errno) {
      case EINVAL:
        // Cropping not supported. */
        break;
      default:
        // Errors ignored. */
        break;
    }
  }
  sleep(1);

  // Select standard */
  printf("...select standard of %s ...\n", dev_name_.c_str());
  // standard = V4L2_STD_NTSC;
  standard = V4L2_STD_PAL;
  if (-1 == xioctl(fd, VIDIOC_S_STD, &standard)) {
    perror("VIDIOC_S_STD");
    exit(EXIT_FAILURE);
  }
  sleep(1);

  // Select input */
  printf("...select input channel of %s ...\n\n", dev_name_.c_str());
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

  // Note VIDIOC_S_FMT may change width and height. */

  // Buggy driver paranoia. */
  min = fmt.fmt.pix.width * BYTESPERPIXEL;
  if (fmt.fmt.pix.bytesperline < min) fmt.fmt.pix.bytesperline = min;
  min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
  if (fmt.fmt.pix.sizeimage < min) fmt.fmt.pix.sizeimage = min;

  switch (io_) {
    case IoMethod::IO_METHOD_READ:
      init_read(fmt.fmt.pix.sizeimage);
      break;

    case IoMethod::IO_METHOD_MMAP:
      InitaliseMmap();
      break;

    case IoMethod::IO_METHOD_USERPTR:
      InitaliseUserp(fmt.fmt.pix.sizeimage);
      break;
  }
}

void V4l2_Capture_YUYV::CloseDevice(void) {
  if (-1 == close(fd)) errno_exit("close");

  fd = -1;
}

void V4l2_Capture_YUYV::OpenDevice(void) {
  struct stat st;

  if (-1 == stat(dev_name_.c_str(), &st)) {
    fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name_.c_str(), errno, strerror(errno));
    exit(EXIT_FAILURE);
  }

  if (!S_ISCHR(st.st_mode)) {
    fprintf(stderr, "%s is no device\n", dev_name_.c_str());
    exit(EXIT_FAILURE);
  }

  fd = open(dev_name_.c_str(), O_RDWR | O_NONBLOCK, 0);

  if (-1 == fd) {
    fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name_.c_str(), errno, strerror(errno));
    exit(EXIT_FAILURE);
  }
}
