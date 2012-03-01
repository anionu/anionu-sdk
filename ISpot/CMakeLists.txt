include_dependency(Poco REQUIRED)
include_dependency(OpenSSL REQUIRED)
include_dependency(FFmpeg REQUIRED)
include_dependency(OpenCV REQUIRED)
include_dependency(RtAudio REQUIRED)
include_dependency(JsonCpp REQUIRED)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
    
define_libsourcey_module(ISpot Base Net Media ICE RTP STUN TURN SDP JSON Symple)
