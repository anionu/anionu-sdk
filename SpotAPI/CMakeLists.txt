ask_build_sourcey_module(SpotAPI)
if(BUILD_MODULES AND BUILD_MODULE_SpotAPI) 
  include_dependency(Poco REQUIRED)
  include_dependency(OpenSSL REQUIRED)
  include_dependency(OpenCV REQUIRED)
  include_dependency(RtAudio REQUIRED)
  include_dependency(JsonCpp REQUIRED)

  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/Anionu/Spot/API")
      
  define_sourcey_module(SpotAPI Base Net Media ICE RTP STUN TURN SDP JSON Symple Util)
endif()
