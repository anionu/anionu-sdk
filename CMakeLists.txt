include(SpotPlugin.cmake REQUIRED)

set(Anionu_API_USERNAME "username" CACHE PATH "Anionu API Username")
set(Anionu_API_PASSWORD "password" CACHE PATH "Anionu API Password")
set(Anionu_API_USERNAME ${Anionu_API_USERNAME} PARENT_SCOPE)
set(Anionu_API_PASSWORD ${Anionu_API_PASSWORD} PARENT_SCOPE)

add_subdirectory(Anionu)

ask_build_sourcey_module(Spot)
if(BUILD_MODULES AND BUILD_MODULE_Spot) 
  add_subdirectory(SpotAPI)
endif()

ask_build_sourcey_application(SpotPlugins)
if(BUILD_APPLICATIONS AND BUILD_APPLICATION_SpotPlugins) 
  add_subdirectory(SpotPlugins)
endif()
