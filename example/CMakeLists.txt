project(examples)

add_subdirectory(common)
add_subdirectory(simple)
add_subdirectory(rtp_transmit)
add_subdirectory(rtp_sap_transmit)
add_subdirectory(rtp_receive)
add_subdirectory(rtp_sap_receive)
add_subdirectory(sap_announcer)
add_subdirectory(sap_listener)
add_subdirectory(v4l2)
if (BUILD_QT6)
add_subdirectory(qt6)
endif()

# Copy testcard.png to the build directory so that it can be found when running example
file(COPY ${CMAKE_SOURCE_DIR}/images/testcard.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
