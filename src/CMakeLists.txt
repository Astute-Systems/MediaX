option(BUILD_CUDA OFF)

if (BUILD_CUDA)
  project(mediax LANGUAGES CXX CUDA)
else()
  project(mediax LANGUAGES CXX)
endif()

include(FindPkgConfig)
include(GNUInstallDirs)


if (${CMAKE_SYSTEM_NAME} MATCHES "MSYS")
  message("Detected MSYS2...")
  set(MSYS_LIBS ws2_32 mingwex)
endif()

add_library(mediax SHARED rtpvraw_payloader.cc rtpvraw_depayloader.cc rtp_utils.cc sap_announcer.cc sap_listener.cc colourspace.cc colourspace_cuda.cu)
pkg_check_modules(SWSCALE REQUIRED libswscale)
target_link_libraries(mediax png pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS})
target_include_directories(mediax PUBLIC ${SWSCALE_INCLUDE_DIRS})
set_target_properties(mediax PROPERTIES SOVERSION 1)
set_target_properties(mediax PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_property(TARGET mediax PROPERTY CUDA_SEPARABLE_COMPILATION ON)

add_library(mediax_static SHARED rtpvraw_payloader.cc rtpvraw_depayloader.cc rtp_utils.cc sap_announcer.cc sap_listener.cc colourspace.cc)
target_link_libraries(mediax_static png pthread ${SWSCALE_LIBRARIES} ${MSYS_LIBS})
target_include_directories(mediax_static PUBLIC ${SWSCALE_INCLUDE_DIRS})
set_target_properties(mediax_static PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

# Install 
install(TARGETS mediax DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS mediax_static DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${CMAKE_SOURCE_DIR}/src/*.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
