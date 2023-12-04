include(GNUInstallDirs)

set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/mediax;
    CACHE PATH "Location of header files" )

set(INCLUDE_QT6_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/mediax/qt6;
    CACHE PATH "Location of Qt6 wrapper header files" )

message(STATUS "Include install dir: ${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR}")

set(LIBRARIES_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
    CACHE PATH "Location of libraries" )
message(STATUS "Libraries install dir: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

set(LIBRARIES ${LIBRARIES_INSTALL_DIR}/libmediax.so
              ${LIBRARIES_INSTALL_DIR}/libmediax.a
              ${LIBRARIES_INSTALL_DIR}/libmediax.so.1
              ${LIBRARIES_INSTALL_DIR}/libmediax.so.1.0.0
              ${LIBRARIES_INSTALL_DIR}/libmediax.so.1.0.0)

include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/MediaXConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/MediaXConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MediaX
  PATH_VARS INCLUDE_INSTALL_DIR INCLUDE_QT6_INSTALL_DIR LIBRARIES_INSTALL_DIR) 

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/MediaXConfigVersion.cmake
  VERSION ${MEDIAX_VERSION}
  COMPATIBILITY SameMajorVersion )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MediaXConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/MediaXConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MediaX )