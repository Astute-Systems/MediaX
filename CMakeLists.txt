cmake_minimum_required(VERSION 3.0…3.16)
project(mediax)

# enable testing functionality
enable_testing()

include(scripts/cmake/version.cmake)
include(scripts/cmake/cpp.cmake)
include(scripts/cmake/cpplint.cmake)
# include(scripts/cmake/linters.cmake)
include(scripts/cmake/cpack.cmake)
include(scripts/cmake/githooks.cmake)
include(scripts/cmake/code_coverage.cmake)

add_subdirectory(src)
option(EXAMPLES "Build examples" OFF)
if (EXAMPLES) 
    add_subdirectory(example)
endif()

if (BUILD_TESTING) 
    add_subdirectory(tests)
endif()

# Install 
include(GNUInstallDirs)

install(TARGETS mediax DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS mediax_static DESTINATION ${CMAKE_INSTALL_LIBDIR})
file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
install(FILES ${HEADER_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# install headers
include(GNUInstallDirs)
file(GLOB_RECURSE MEDIAX_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/rtp/*.h)
install(FILES ${MEDIAX_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mediax/rtp)
file(GLOB_RECURSE MEDIAX_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/raw/*.h)
install(FILES ${MEDIAX_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mediax/raw)
file(GLOB_RECURSE MEDIAX_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/h264/*.h)
install(FILES ${MEDIAX_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mediax/h264)
file(GLOB_RECURSE MEDIAX_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/sap/*.h)
install(FILES ${MEDIAX_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mediax/sap)
file(GLOB_RECURSE MEDIAX_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.h)
install(FILES ${MEDIAX_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mediax/utils)
file(GLOB_RECURSE MEDIAX_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/v4l2/*.h)
install(FILES ${MEDIAX_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mediax/v4l2)
#install package config file
install(FILES ${CMAKE_BINARY_DIR}/mediax.pc DESTINATION  ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

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