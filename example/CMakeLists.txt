project(examples)

find_package(gflags REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-3.0 REQUIRED)
pkg_check_modules(CAIRO cairo REQUIRED)

include_directories(${GTKMM_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS} ${CAIRO_LIBRARY_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(transmit-example transmit-example.cc pngget.cc)
target_link_libraries(transmit-example mediax gflags)

add_executable(recieve-example recieve-example.cc pngget.cc)
target_link_libraries(recieve-example mediax gflags ${GTKMM_LIBRARIES} ${CAIRO_LIBRARIES} pthread)

# Copy testcard.png to the build directory so that it can be found when running example
file(COPY ${CMAKE_SOURCE_DIR}/images/testcard.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
