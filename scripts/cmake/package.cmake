include(GNUInstallDirs)
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/MediaX
    CACHE PATH "Location of header files" )
set(SYSCONFIG_INSTALL_DIR ${CMAKE_INSTALL_SYSCONFDIR}/MediaX
    CACHE PATH "Location of configuration files" )
#...
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_SOURCE_DIR}/scripts/cmake/MediaXConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/MediaXConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MediaX
  PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/MediaXConfigVersion.cmake
  VERSION "${MEDIAX_MAJOR_VERSION}.${MEDIAX_MINOR_VERSION}.${MEDIAX_PATCH_VERSION}"
  COMPATIBILITY SameMajorVersion )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MediaXConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/MediaXConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mediax )