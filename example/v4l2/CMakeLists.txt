project(v4l2)

include_directories(${CMAKE_SOURCE_DIR}/example/v4l2)

add_executable(v4l2c++ main.cc utils.cc capture_yuyv.cc)

add_executable(v4l2  capture_yuyv.c )

# Link with gflags
target_link_libraries(v4l2c++ gflags)