include(SpotPlugin.cmake REQUIRED)

set_option(Anionu_API_USERNAME "Anionu API username" "username")
set_option(Anionu_API_PASSWORD "Anionu API password" "password")

add_subdirectory(Anionu)
add_subdirectory(SpotAPI)
add_subdirectory(SpotPlugins)


#set(Anionu_API_USERNAME "username" CACHE PATH "Anionu API Username")
#set(Anionu_API_PASSWORD "password" CACHE PATH "Anionu API Password")
#set(Anionu_API_USERNAME ${Anionu_API_USERNAME} PARENT_SCOPE)
#set(Anionu_API_PASSWORD ${Anionu_API_PASSWORD} PARENT_SCOPE)
#ask_build_sourcey_module(SpotAPI)
#if(BUILD_MODULES AND BUILD_MODULE_SpotAPI) 
#endif()
