include(SpotPlugin.cmake REQUIRED)

set(Anionu_API_USERNAME "test" CACHE PATH "Anionu API Username")
set(Anionu_API_PASSWORD "password" CACHE PATH "Anionu API Password")

if(BUILD_MODULES)
  add_subdirectory(Anionu)
  add_subdirectory(ISpot)
endif()

if(BUILD_APPLICATIONS)
  if(MSVC)
    add_subdirectory(SpotPlugins)
  endif()
endif()
