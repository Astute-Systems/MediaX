project(rtp-example)

find_package(gflags REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(rtp-example example.cc pngget.cc)
target_link_libraries(rtp-example mediax gflags)

# Copy testcard.png to the build directory so that it can be found when running example
file(COPY ${CMAKE_SOURCE_DIR}/images/testcard.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
