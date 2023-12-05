find_package(gflags REQUIRED)
find_package(glog REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)
pkg_check_modules(GTKMM gtkmm-3.0 REQUIRED)
pkg_check_modules(CAIRO cairo REQUIRED)
pkg_check_modules(SWSCALE REQUIRED libswscale)

include_directories(${GTKMM_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/include ${GST_INCLUDE_DIRS})

link_directories(${GTKMM_LIBRARY_DIRS} ${CAIRO_LIBRARY_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(rtp-receive receive-example.cc)
target_link_libraries(rtp-receive mediax_static example_helpers ${GST_LIBRARIES} ${GLOG_LIBRARIES}  ${GFLAGS_LIBRARIES} ${GTKMM_LIBRARIES} ${CAIRO_LIBRARIES} png)
