set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# CMake policy
cmake_policy(SET CMP0086 NEW)
cmake_policy(SET CMP0078 NEW)

# Enable C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fPIC -Wall -Werror -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-unused-result -Wno-unused-label -Wno-unused-value -Wno-unused-local-typedefs -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-unused-result -Wno-unused-label -Wno-unused-value -Wno-unused-local-typedefs -Wno-unknown-pragmas -Wno-attributes -Wno-deprecated-declarations -Wno-ignored-attributes -Wno-implicit-fallthrough")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
endif()

# Options
option(GST_SUPPORTED "GStreamer support" ON)
if (GST_SUPPORTED) 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGST_SUPPORTED")
endif()

option(VAAPI_SUPPORTED "Intel VAAPI support" OFF)
if (VAAPI_SUPPORTED) 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVAAPI_SUPPORTED")
endif()


option(NVENC_SUPPORTED "Nvidia NVENC support" OFF)
if (NVENC_SUPPORTED) 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNVENC_SUPPORTED")
endif()

if(CMAKE_BUILD_TYPE)
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build type: Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

