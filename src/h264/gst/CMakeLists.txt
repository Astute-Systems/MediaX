
if (GST_SUPPORTED)
  add_subdirectory(nvidia)
  add_subdirectory(vaapi)
  add_subdirectory(omx)
  add_subdirectory(x264)
  add_subdirectory(open)
endif()