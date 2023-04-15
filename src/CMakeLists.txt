project(mediax)
include(FindPkgConfig)

if (${CMAKE_SYSTEM_NAME} MATCHES "MSYS")
  message("Detected MSYS2...")
  set(MSYS_LIBS ws2_32 mingwex)
endif()

add_library(mediax SHARED rtp_stream.cc )
pkg_check_modules(SWSCALE REQUIRED libswscale)
target_link_libraries(mediax png pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS})
target_include_directories(mediax PUBLIC ${SWSCALE_INCLUDE_DIRS})
set_target_properties(mediax PROPERTIES SOVERSION 1)

# Install 
install(TARGETS mediax DESTINATION /usr/local/lib)
