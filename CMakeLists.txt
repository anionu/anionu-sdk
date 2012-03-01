include(SpotPlugin.cmake REQUIRED)

if(BUILD_MODULES)
  add_subdirectory(Anionu)
  add_subdirectory(ISpot)
endif()

if(BUILD_APPLICATIONS)
  #if(MSVC)
  #  add_subdirectory(SpotPlugins)
  #endif()
endif()