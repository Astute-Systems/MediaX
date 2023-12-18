set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# CMake policy
cmake_policy(SET CMP0086 NEW)
cmake_policy(SET CMP0078 NEW)
# if CMAKE version > 3.27
if ( ${CMAKE_VERSION} VERSION_GREATER "3.27.0")
  cmake_policy(SET CMP0148 NEW)
endif()

# Check cmake version

# cmake_policy(SET CMP0148 NEW)
# Enable C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fPIC -Wall -Werror -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-unused-result -Wno-unused-label -Wno-unused-value -Wno-unused-local-typedefs -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-unused-result -Wno-unused-label -Wno-unused-value -Wno-unused-local-typedefs -Wno-unknown-pragmas -Wno-attributes -Wno-deprecated-declarations -Wno-ignored-attributes -Wno-implicit-fallthrough")
# If debug build, enable debug flags
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fPIC -fprofile-arcs -ftest-coverage -Wall -Werror -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-unused-result -Wno-unused-label -Wno-unused-value -Wno-unused-local-typedefs -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-unused-result -Wno-unused-label -Wno-unused-value -Wno-unused-local-typedefs -Wno-unknown-pragmas -Wno-attributes -Wno-deprecated-declarations -Wno-ignored-attributes -Wno-implicit-fallthrough")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage -DNDEBUG")
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

# BUILD_CUDA
option(BUILD_CUDA "Build CUDA" OFF)
if (BUILD_CUDA)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILD_CUDA")
endif()

# BUILD_QT6 Wrappers
option(BUILD_QT6 "Build QT6 Wrappers" OFF)
if (BUILD_QT6)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILD_QT6")
endif()

# BUILD_RUST
option(BUILD_RUST "Build Rust" OFF)
if (BUILD_RUST)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILD_RUST")
endif()


if(CMAKE_BUILD_TYPE)
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build type: Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

