ask_build_sourcey_module(ISpot)
if(BUILD_MODULE_ISpot) 
  include_dependency(Poco REQUIRED)
  include_dependency(OpenSSL REQUIRED)
  include_dependency(OpenCV REQUIRED)
  include_dependency(RtAudio REQUIRED)
  include_dependency(JsonCpp REQUIRED)

  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
      
  define_sourcey_module(ISpot Base Net Media ICE RTP STUN TURN SDP JSON Symple)
endif()
