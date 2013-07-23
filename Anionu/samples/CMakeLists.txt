# ----------------------------------------------------------------------------
#  CMake file for samples. See root CMakeLists.txt 
#
# ----------------------------------------------------------------------------

add_subdirectory(AnionuAPIConsole)
  
if(BUILD_ALPHA)
  add_subdirectory(MotionDetector)
endif()
