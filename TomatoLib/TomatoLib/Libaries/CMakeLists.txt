# Add GLFW3
add_subdirectory(glfw)

# Add freetype-gl
# We have to disable doxygen and demos:
# - Documentation is available online.
# - We don't urgently need demos, plus they require AntTweakBar library to be installed.
set(freetype-gl_BUILD_APIDOC OFF CACHE BOOL "Ignore doxygen documentation")
set(freetype-gl_BUILD_DEMOS OFF CACHE BOOL "Ignore demos")
add_subdirectory(freetype-gl)

# Add VTFLib
add_subdirectory(VTFLib)
