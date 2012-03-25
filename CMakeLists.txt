include(SpotPlugin.cmake REQUIRED)

set(Anionu_API_USERNAME "test" CACHE PATH "Anionu API Username")
set(Anionu_API_PASSWORD "password" CACHE PATH "Anionu API Password")
set(Anionu_API_USERNAME ${Anionu_API_USERNAME} PARENT_SCOPE)
set(Anionu_API_PASSWORD ${Anionu_API_PASSWORD} PARENT_SCOPE)


if(BUILD_MODULES)
  add_subdirectory(Anionu)
  add_subdirectory(ISpot)
endif()

if(BUILD_APPLICATIONS)
  if(MSVC)
    add_subdirectory(SpotPlugins)
  endif()
endif()
