
if (GST_SUPPORTED)
  # Use GStreamer for hardware accelerated video decoding
  add_subdirectory(gst)
endif()

if (VAAPI_SUPPORTED)
  # Use Intel Video Acceleration API (VAAPI) for hardware accelerated video decoding
  add_subdirectory(vaapi)
endif()

if (NVENC_SUPPORTED)
  # Use Nvidia Encoder (NVENC) for hardware accelerated video decoding
  add_subdirectory(nvenc)
endif()

if (!GST_SUPPORTED AND !VAAPI_SUPPORTED AND !NVENC_SUPPORTED)
  # Use native video decoding for software x264 decoding
  add_subdirectory(native)
endif()