cmake_minimum_required(VERSION 3.0…3.16)
project(mediax)

include(CTest)

# enable testing functionality
enable_testing()

add_subdirectory(src)

include(scripts/cmake/version.cmake)
include(scripts/cmake/cpp.cmake)
include(scripts/cmake/cpack.cmake)

option(EXAMPLES "Build examples" OFF)
if (EXAMPLES) 
    add_subdirectory(example)
endif()

if (BUILD_TESTING) 
    add_subdirectory(tests)
endif()
