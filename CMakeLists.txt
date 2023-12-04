cmake_minimum_required(VERSION 3.10…3.16)
project(mediax)

# enable testing functionality
enable_testing()

include(scripts/cmake/version.cmake)
include(scripts/cmake/doxygen.cmake)
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

# Header install function from dir
function (install_headers_from_dir HEADER_DIR DESTINATION_DIR)
    file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER_DIR}/*.h)
    install(FILES ${HEADER_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${DESTINATION_DIR})
endfunction()

# install headers
include(GNUInstallDirs)
install_headers_from_dir(src/rtp mediax/rtp)
install_headers_from_dir(src/uncompressed mediax/uncompressed)
install_headers_from_dir(src/h264 mediax/h264)
install_headers_from_dir(src/h264/nvidia mediax/h264/nvidia)
install_headers_from_dir(src/h264/vaapi mediax/h264/vaapi)
install_headers_from_dir(src/h264/gst/vaapi mediax/h264/gst/vaapi)
install_headers_from_dir(src/h265/gst/vaapi mediax/h265/gst/vaapi)
install_headers_from_dir(src/h264/gst/nvidia mediax/h264/gst/nvidia)
install_headers_from_dir(src/sap mediax/sap)
install_headers_from_dir(src/utils mediax/utils)
install_headers_from_dir(src/v4l2 mediax/v4l2)
install_headers_from_dir(src/qt6 mediax/qt6)
install_headers_from_dir(src/wrappers mediax/wrappers)
install(FILES ${CMAKE_SOURCE_DIR}/src/qt6/QSap ${CMAKE_SOURCE_DIR}/src/qt6/QRtp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mediax/qt)


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