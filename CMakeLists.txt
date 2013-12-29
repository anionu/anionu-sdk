include(SpotPlugin.cmake REQUIRED)
include(AnionuUtilities.cmake REQUIRED)

set(Anionu_API_USERNAME "username" CACHE PATH "Anionu API Username")
set(Anionu_API_KEY "password" CACHE PATH "Anionu API Password")
set(Anionu_API_USERNAME ${Anionu_API_USERNAME} PARENT_SCOPE)
set(Anionu_API_KEY ${Anionu_API_KEY} PARENT_SCOPE)

add_subdirectory(anionu)
add_subdirectory(spotapi)
add_subdirectory(spotplugins)