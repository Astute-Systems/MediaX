project(examples)

find_package(gflags REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0)
pkg_check_modules(GTKMM gtkmm-3.0 REQUIRED)
pkg_check_modules(CAIRO cairo REQUIRED)
pkg_check_modules(SWSCALE REQUIRED libswscale)


include_directories(${GTKMM_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/include ${GST_INCLUDE_DIRS})

link_directories(${GTKMM_LIBRARY_DIRS} ${CAIRO_LIBRARY_DIRS} ${GST_LIBRARY_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(transmit-example transmit-example.cc example_helpers.cc pngget.cc)
target_link_libraries(transmit-example mediax_static gflags ${GST_LIBRARIES} )

add_executable(receive-example receive-example.cc example_helpers.cc pngget.cc)
target_link_libraries(receive-example mediax glog gflags ${GTKMM_LIBRARIES} ${CAIRO_LIBRARIES} ${GST_LIBRARIES} ${SWSCALE_LIBRARIES} png)

add_executable(sap-example sap-example.cc)
target_link_libraries(sap-example mediax ${GST_LIBRARIES} glog gflags ${GTKMM_LIBRARIES} ${CAIRO_LIBRARIES} ${GST_LIBRARIES} ${SWSCALE_LIBRARIES})

add_executable(sap-listener sap-listener.cc)
target_link_libraries(sap-listener mediax ${GST_LIBRARIES} glog gflags ${GTKMM_LIBRARIES} ${CAIRO_LIBRARIES} ${GST_LIBRARIES} ${SWSCALE_LIBRARIES})

# Copy testcard.png to the build directory so that it can be found when running example
file(COPY ${CMAKE_SOURCE_DIR}/images/testcard.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
