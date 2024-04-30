#include <capture.h>
#include <capture_yuyv.h>

std::string dev_name;
static IoMethod io = IoMethod::IO_METHOD_MMAP;

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
  dev_name = "/dev/video0";
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

  V4l2_Capture_YUYV capture(dev_name);

  for (;;) {
    int index;
    int c;

    c = getopt_long(argc, argv, short_options, long_options, &index);

    if (-1 == c) break;

    switch (c) {
      case 0:  // getopt_long() flag */
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
        io = IoMethod::IO_METHOD_MMAP;
        break;

      case 'r':
        io = IoMethod::IO_METHOD_READ;
        break;

      case 'u':
        io = IoMethod::IO_METHOD_USERPTR;
        break;

      default:
        usage(stderr, argc, argv);
        exit(EXIT_FAILURE);
    }
  }

  capture.OpenDevice();

  capture.InitaliseDevice();

  capture.StartDevice();

  capture.Run();

  capture.StopDevice();

  capture.UninitaliseDevice();

  capture.CloseDevice();

  exit(EXIT_SUCCESS);

  return 0;
}
