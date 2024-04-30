project(v4l2)

include_directories(${CMAKE_SOURCE_DIR}/example/v4l2)

message (STATUS "CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR}/example/v4l2)
add_executable(v4l2c++ capture_yuyv.cc)
add_executable(v4l2 capture_yuyv.c)
