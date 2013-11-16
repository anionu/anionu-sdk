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
    
    include_directories(${LibSourcey_INCLUDE_DIRS})    
    link_directories(${LibSourcey_LIBRARY_DIRS})
    
    include_sourcey_modules(uv base anionu spotapi ${ARGN}) # Media Net JSON Media  HTTP    
    include_directories(
      "${LibSourcey_INSTALL_DIR}/lib"
      "${CMAKE_SOURCE_DIR}/src/AnionuSDK/SpotAPI/include")      
    
    add_library(${name} MODULE ${lib_srcs} ${lib_hdrs})
      
    #status("  Linking Spot plugin ${name}")    
    #status("    Libraries:               ${LibSourcey_INCLUDE_LIBRARIES}")    
    #status("    Library Dirs:            ${LibSourcey_LIBRARY_DIRS}")    
    #status("    Include Dirs:            ${LibSourcey_INCLUDE_DIRS}")  
    #status("    Dependencies:            ${LibSourcey_BUILD_DEPENDENCIES}")  
  
    target_link_libraries(${name} ${LibSourcey_INCLUDE_LIBRARIES}) 
    
    if(ENABLE_SOLUTION_FOLDERS)
      set_target_properties(${name} PROPERTIES FOLDER "plugins")
    endif()
    set_target_properties(${name} PROPERTIES DEBUG_POSTFIX "${LibSourcey_DEBUG_POSTFIX}")
    
    if (NOT ${name}_EXECUTABLE_NAME)
      set(${name}_EXECUTABLE_NAME ${name})
    endif()
    if (NOT ${name}_DEBUG_POSTFIX AND NOT ${name}_DEBUG_POSTFIX STREQUAL "")
      set(${name}_DEBUG_POSTFIX ${LibSourcey_DEBUG_POSTFIX})
    endif()
    set_target_properties(${name} PROPERTIES
      OUTPUT_NAME ${${name}_EXECUTABLE_NAME}
      DEBUG_POSTFIX "${${name}_DEBUG_POSTFIX}")
    
    install(TARGETS ${name} 
      DESTINATION "${CMAKE_INSTALL_PREFIX}/spot/plugins/${name}" COMPONENT main) 
        
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
