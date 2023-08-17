cmake_minimum_required(VERSION 3.0…3.16)
project(mediax)

include(CTest)

# enable testing functionality
enable_testing()

add_subdirectory(src)

include(scripts/cmake/version.cmake)
include(scripts/cmake/cpp.cmake)
include(scripts/cmake/cpplint.cmake)
# include(scripts/cmake/linters.cmake)
include(scripts/cmake/cpack.cmake)

option(EXAMPLES "Build examples" OFF)
if (EXAMPLES) 
    add_subdirectory(example)
endif()

if (BUILD_TESTING) 
    add_subdirectory(tests)
endif()

# cpplint setup
cpplint_add_subdirectory(src)

# Uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()