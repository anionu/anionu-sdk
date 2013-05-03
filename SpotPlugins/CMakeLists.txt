# Dynamically adding subdirectories
if(BUILD_MODULES OR BUILD_APPLICATIONS)    
  subdirlist(subdirs ${CMAKE_CURRENT_SOURCE_DIR})
  foreach(name ${subdirs})
    set(dir "${CMAKE_CURRENT_SOURCE_DIR}/${name}")
    if (EXISTS "${dir}/CMakeLists.txt")
      add_subdirectory(${dir})
    endif()
  endforeach() 
endif()

#add_subdirectory(SurveillanceModePlugin)
#add_subdirectory(RecordingModePlugin)
#add_subdirectory(MediaPlugin)
