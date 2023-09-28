project(examples)

add_subdirectory(common)
add_subdirectory(transmit)
add_subdirectory(receive)
add_subdirectory(sap_announcer)
add_subdirectory(sap_listener)

# Copy testcard.png to the build directory so that it can be found when running example
file(COPY ${CMAKE_SOURCE_DIR}/images/testcard.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
