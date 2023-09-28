
option(GST_SUPPORTED "GStreamer support" ON)
if (GST_SUPPORTED)
  add_subdirectory(gst)
endif()