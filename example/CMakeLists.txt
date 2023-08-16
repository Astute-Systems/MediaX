project(examples)

find_package(gflags REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-3.0 REQUIRED)
pkg_check_modules(CAIRO cairo REQUIRED)

include_directories(${GTKMM_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/include)

link_directories(${GTKMM_LIBRARY_DIRS} ${CAIRO_LIBRARY_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(transmit-example transmit-example.cc pngget.cc)
target_link_libraries(transmit-example mediax gflags)

add_executable(receive-example receive-example.cc pngget.cc)
target_link_libraries(receive-example mediax gflags ${GTKMM_LIBRARIES} ${CAIRO_LIBRARIES} pthread)

add_executable(sap-example sap-example.cc)
target_link_libraries(sap-example mediax pthread)

add_executable(sap-listener sap-listener.cc)
target_link_libraries(sap-listener mediax pthread)

# Copy testcard.png to the build directory so that it can be found when running example
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/images/testcard.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
