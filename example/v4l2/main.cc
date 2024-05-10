#include <capture.h>
#include <capture_yuyv.h>
#include <gflags/gflags.h>

#include <iostream>

static IoMethod io = IoMethod::IO_METHOD_MMAP;

DEFINE_string(device, "/dev/video", "Video device name");
DEFINE_bool(help, false, "Print this message");
DEFINE_bool(mmap, false, "Use memory mapped buffers");
DEFINE_bool(read, false, "Use read() calls");
DEFINE_bool(userp, false, "Use application allocated buffers");

std::string usage() {
  return "Usage: [options]\n\n"
         "Options:\n"
         "--device name   Video device name [/dev/video]\n"
         "--help          Print this message\n"
         "--mmap          Use memory mapped buffers\n"
         "--read          Use read() calls\n"
         "--userp         Use application allocated buffers\n"
         "\n";
}

int main(int argc, char **argv) {
  gflags::SetUsageMessage(usage());
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_help) {
    gflags::ShowUsageWithFlags(argv[0]);
    return 0;
  }

  V4l2_Capture_YUYV capture(FLAGS_device);
  if (FLAGS_mmap) {
    io = IoMethod::IO_METHOD_MMAP;
  } else if (FLAGS_read) {
    io = IoMethod::IO_METHOD_READ;
  } else if (FLAGS_userp) {
    io = IoMethod::IO_METHOD_USERPTR;
  } else {
    usage();
    exit(EXIT_FAILURE);
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
