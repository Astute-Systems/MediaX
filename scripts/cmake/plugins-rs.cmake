include(ExternalProject)

message (STATUS "Added plugins-rs to external submodules")

# ------------------------------------------------------------------------------
# mediax
# ------------------------------------------------------------------------------
ExternalProject_Add(
  plugins-rs
  GIT_REPOSITORY      https://gitlab.freedesktop.org/gstreamer/gst-plugins-rs
  GIT_TAG             gstreamer-1.24.1 
  GIT_SHALLOW         1
  CMAKE_ARGS          -DBUILD_TESTING=OFF -DEXAMPLES=OFF -DBUILD_QT6=ON
  PREFIX              ${CMAKE_BINARY_DIR}/_deps/plugins-rs/prefix
  TMP_DIR             ${CMAKE_BINARY_DIR}/_deps/plugins-rs/tmp
  STAMP_DIR           ${CMAKE_BINARY_DIR}/_deps/plugins-rs/stamp
  DOWNLOAD_DIR        ${CMAKE_BINARY_DIR}/_deps/plugins-rs/download
  SOURCE_DIR          ${CMAKE_BINARY_DIR}/_deps/plugins-rs/src
  BUILD_IN_SOURCE     1
  CONFIGURE_COMMAND   ""
  BUILD_COMMAND       cargo build -p gst-plugin-rtp --release
  INSTALL_COMMAND     ${CMAKE_COMMAND} -E copy
                      ${CMAKE_BINARY_DIR}/_deps/plugins-rs/src/target/release/deps/libgstrsrtp.so
                      ${CMAKE_BINARY_DIR}/_deps/install/lib/x86_64-linux-gnu/gstreamer-1.0/libgstrsrtp.so
  UPDATE_DISCONNECTED 1
  BUILD_ALWAYS        0     
)

install(FILES ${CMAKE_BINARY_DIR}/_deps/install/lib/x86_64-linux-gnu/gstreamer-1.0/libgstrsrtp.so
  DESTINATION /lib/x86_64-linux-gnu/gstreamer-1.0)