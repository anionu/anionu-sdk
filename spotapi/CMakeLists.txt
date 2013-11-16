# SpotAPI is not header only, nothing to build
ask_build_sourcey_module(spotapi)
if(BUILD_MODULES AND BUILD_MODULE_spotapi)
  #include_dependency(OpenSSL REQUIRED)
  #include_dependency(OpenCV REQUIRED) # Remove for base API only
  #include_dependency(RtAudio REQUIRED)
  #include_dependency(JsonCpp REQUIRED)

  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
      
  define_sourcey_module(spotapi base net media json symple util)
endif()