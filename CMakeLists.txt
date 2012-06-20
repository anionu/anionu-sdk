include(SpotPlugin.cmake REQUIRED)

set(Anionu_API_USERNAME "test" CACHE PATH "Anionu API Username")
set(Anionu_API_PASSWORD "password" CACHE PATH "Anionu API Password")
set(Anionu_API_USERNAME ${Anionu_API_USERNAME} PARENT_SCOPE)
set(Anionu_API_PASSWORD ${Anionu_API_PASSWORD} PARENT_SCOPE)

if(BUILD_APPLICATIONS AND MSVC)
  set(BUILD_SPOT_APPLICATIONS ON CACHE BOOL "Build Spot applications")
else()
  set(BUILD_SPOT_APPLICATIONS OFF CACHE BOOL "Build Spot applications")
endif()

if(BUILD_MODULES)
  add_subdirectory(Anionu)
  add_subdirectory(ISpot)
endif()

if(BUILD_SPOT_APPLICATIONS)
  add_subdirectory(SpotPlugins)
endif()
