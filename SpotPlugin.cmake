#
### Macro: define_spot_executable
#
# This template defines a Spot plug-in.
#
macro(define_spot_plugin name)
            
    project(${name})

    file(GLOB lib_srcs "*.cpp")
    file(GLOB lib_hdrs "*.h*")
    
    source_group("Src" FILES ${lib_srcs})
    source_group("Include" FILES ${lib_hdrs})
    
    include_dependency(Poco REQUIRED)
    include_dependency(OpenSSL REQUIRED)
    include_dependency(JsonCpp REQUIRED)
    include_dependency(FFmpeg REQUIRED)
    include_dependency(OpenCV REQUIRED)
    include_dependency(RtAudio REQUIRED)
    
    include_directories(${LibSourcey_INCLUDE_DIRS})    
    link_directories(${LibSourcey_LIBRARY_DIRS})
    
    include_sourcey_modules(Base Net JSON Media HTTP Anionu ISpot HTTP Symple ${ARGN})
    include_directories(
      "${LibSourcey_INSTALL_DIR}/lib"
      "${CMAKE_SOURCE_DIR}/projects/Anionu/ISpot/include")

    add_library(${name} MODULE ${lib_srcs} ${lib_hdrs})
    
    # Plug-ins can not have a debug prefix
    set_target_properties(${name} PROPERTIES DEBUG_POSTFIX "")
    
    target_link_libraries(${name} ${LibSourcey_INCLUDE_LIBRARIES}) 
    
    install(TARGETS ${name} 
      DESTINATION "${CMAKE_INSTALL_PREFIX}/Spot/plugins/${name}" COMPONENT main) 
        
endmacro()

           
    
    #include_directories("${CMAKE_SOURCE_DIR}/projects/Anionu/Anionu/include")
    #include_directories("${CMAKE_SOURCE_DIR}/projects/Anionu/ISpot/include")
    #target_link_libraries(${name} debug ${lib})
    #target_link_libraries(${name} optimized ${lib})
    
    # Include extra modules
    #foreach(module ${ARGN})
    #    include_directories("${CMAKE_SOURCE_DIR}/modules/${module}/include")
    #endforeach()
    #set(install_path "${CMAKE_INSTALL_PREFIX}/Spot/plugins/${name}")
    #message("LibSourcey_DEBUG_LIBS: ${LibSourcey_DEBUG_LIBS}")
    #message("LibSourcey_RELEASE_LIBS: ${LibSourcey_RELEASE_LIBS}")
    #set(manifest "${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml")
    #ADD_CUSTOM_COMMAND(
    #    TARGET ${name}
    #    POST_BUILD
    #    COMMAND ${CMAKE_COMMAND} -E copy "${manifest}" "${install_path}"
    #)
    #get_filename_component(manifest_name "${manifest}" NAME)
    #install(FILES "${install_path}/${manifest_name}" DESTINATION "${install_path}" COMPONENT main)
    
    #foreach(lib IN LISTS LibSourcey_DEBUG_LIBS)
    #message("lib: ${lib}")
    #    target_link_libraries(${name} debug ${lib})
    #endforeach()
    #foreach(lib IN LISTS LibSourcey_RELEASE_LIBS)
    #    target_link_libraries(${name} optimized ${lib})
    #endforeach()   
        
    #include_directories(${LibSourcey_INCLUDE_DIRS})    
    #link_directories(${LibSourcey_LIBRARY_DIRS})
    
    #list(APPEND ${LibSourcey_LIBRARY_DIRS} "SourceyISpot${LibSourcey_DLLVERSION}")  
    #include_directories("${CMAKE_SOURCE_DIR}/projects/Symple/include")
    #foreach(module Base Net JSON Media HTTP  Spot HTTP)
    #    include_directories("${CMAKE_SOURCE_DIR}/modules/${module}/include")
    #endforeach() 
