# Add GLFW3
add_subdirectory(glfw)
list(APPEND tomato_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
list(APPEND tomato_LIBRARIES glfw)
list(APPEND tomato_LIBRARIES ${GLFW_LIBRARIES})

# Add freetype-gl
# We have to disable doxygen and demos:
# - Documentation is available online.
# - We don't urgently need demos, plus they require AntTweakBar library to be installed.
set(freetype-gl_BUILD_APIDOC OFF CACHE BOOL "Ignore doxygen documentation")
set(freetype-gl_BUILD_DEMOS OFF CACHE BOOL "Ignore demos")
add_subdirectory(freetype-gl)
list(APPEND tomato_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/freetype-gl)
list(APPEND tomato_LIBRARIES freetype-gl)

# Add VTFLib
if(NOT WIN32)
	#add_subdirectory(VTFLib)
	#list(APPEND tomato_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/VTFLib/src)
	#list(APPEND tomato_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/VTFLib/src)
	#list(APPEND tomato_LIBRARIES VTFLib13)
else()
	list(APPEND tomato_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/VTFLib_windows/lib)
	list(APPEND tomato_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/VTFLib_windows/lib/x86/VTFLib.lib)
endif()

# Set lists in parent scope
set(tomato_INCLUDES "${tomato_INCLUDES}" PARENT_SCOPE)
set(tomato_LIBRARIES "${tomato_LIBRARIES}" PARENT_SCOPE)
