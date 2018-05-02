cmake_minimum_required(VERSION 3.0)
project(TomatoLib)

cmake_policy(VERSION 3.0)

set(tomato_INCLUDES "")
set(tomato_LIBRARIES "")

add_subdirectory("TomatoLib/TomatoLib/Libaries")

list(APPEND tomato_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/TomatoLib/TomatoLib)

set(presrc "TomatoLib/TomatoLib")

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
	pkg_search_module(GLEW glew)
	pkg_search_module(FT2 freetype2)
else()
	set(GLEW_LIBRARIES "GLEW_LIBRARIES_NOTFOUND" CACHE STRING "Glew libraries")
	set(GLEW_INCLUDE_DIRS "GLEW_INCLUDE_DIRS_NOTFOUND" CACHE STRING "Glew include dirs")

	set(FT2_LIBRARIES "FT2_LIBRARIES_NOTFOUND" CACHE STRING "Freetype 2 libraries")
	set(FT2_INCLUDE_DIRS "FT2_INCLUDE_DIRS_NOTFOUND" CACHE STRING "Freetype 2 include dirs")

	if("${GLEW_LIBRARIES}" STREQUAL "GLEW_LIBRARIES_NOTFOUND")
		message("Glew libraries missing!")
	endif()
	if("${GLEW_INCLUDE_DIRS}" STREQUAL "GLEW_INCLUDE_DIRS_NOTFOUND")
		message("Glew include dirs missing!")
	endif()

	if("${FT2_LIBRARIES}" STREQUAL "FT2_LIBRARIES_NOTFOUND")
		message("Freetype 2 libraries missing!")
	endif()
	if("${FT2_INCLUDE_DIRS}" STREQUAL "FT2_INCLUDE_DIRS_NOTFOUND")
		message("Freetype 2 include dirs missing!")
	endif()
endif()

list(APPEND tomato_INCLUDES ${FT2_INCLUDE_DIRS})
list(APPEND tomato_INCLUDES ${GLEW_INCLUDE_DIRS})

list(APPEND tomato_LIBRARIES ${GLEW_LIBRARIES})
list(APPEND tomato_LIBRARIES ${FT2_LIBRARIES})

add_library(tomato
	${presrc}/MapFormats/Bsp.cpp
	${presrc}/Networking/EzSock.cpp
	${presrc}/Networking/Packet.cpp
	${presrc}/Networking/Connection.cpp
	${presrc}/Networking/Server.cpp
	${presrc}/Async/Async.cpp
	${presrc}/UI/UIFrame.cpp
	${presrc}/UI/UIDropDown.cpp
	${presrc}/UI/UIAdvancedTextEntry.cpp
	${presrc}/UI/UITextEntry.cpp
	${presrc}/UI/UIBase.cpp
	${presrc}/UI/UIPanel.cpp
	${presrc}/UI/UILabel.cpp
	${presrc}/UI/UIFrameBorderless.cpp
	${presrc}/UI/UITexture.cpp
	${presrc}/UI/UISlider.cpp
	${presrc}/UI/UIColorPicker.cpp
	${presrc}/UI/Menus/UIGraph.cpp
	${presrc}/UI/Menus/UIConsole.cpp
	${presrc}/UI/UIScrollPanel.cpp
	${presrc}/UI/UIManager.cpp
	${presrc}/UI/UIButton.cpp
	${presrc}/UI/UIContextMenu.cpp
	${presrc}/UI/UICheckBox.cpp
	${presrc}/UI/UIProgressBar.cpp
	${presrc}/Graphics/Color.cpp
	${presrc}/Graphics/Shader.cpp
	${presrc}/Graphics/Texture.cpp
	${presrc}/Graphics/Render.cpp
	${presrc}/Graphics/Font.cpp
	${presrc}/Graphics/Model.cpp
	${presrc}/Math/Matrix.cpp
	${presrc}/Math/AABB.cpp
	${presrc}/Math/Rectangle.cpp
	${presrc}/Math/Vector2.cpp
	${presrc}/Math/Vector3.cpp
	${presrc}/Math/Vector4.cpp
	${presrc}/ModelFormats/Obj.cpp
	${presrc}/Utilities/Utilities.cpp
	${presrc}/Utilities/HTTP.cpp
	${presrc}/Game/Game.cpp
	${presrc}/Game/Camera.cpp
	${presrc}/Game/Window.cpp
	${presrc}/Game/Content.cpp
	${presrc}/Libaries/LodePNG/lodepng_util.cpp
	${presrc}/Libaries/LodePNG/lodepng.cpp
	${presrc}/Libaries/md5/md5.cpp
)

add_definitions(-DTL_ENABLE_FTGL)

CHECK_INCLUDE_FILE(/opt/vc/include/bcm_host.h RPI)

if(WIN32)
	add_definitions(-DWINDOWS)
elseif(APPLE)
	add_definitions(-DMAC)
else()
	add_definitions(-DLINUX)
	
	add_definitions(-DGLFW_EXPOSE_NATIVE_X11)
	add_definitions(-DGLFW_EXPOSE_NATIVE_GLX)
	
	if (RPI)
		add_definitions(-DUSE_OPENGL_ES -DTL_ENABLE_EGL -DTL_OPENGL_OLD)
	else()
		add_definitions(-DTL_ENABLE_GLFW)
	endif()
endif()

add_definitions(-std=c++11)

include_directories(${tomato_INCLUDES})

if (RPI)
	include_directories(${tomato_INCLUDES}
		"/opt/vc/include"
		"/opt/vc/include/interface/vcos"
		"/opt/vc/include/interface/vmcs_host/linux"
		"/opt/vc/include/interface/vcos/pthreads"
	)
endif()
	
target_link_libraries(tomato
	${tomato_LIBRARIES}
)

if(NOT WIN32)
	target_link_libraries(tomato stdc++)
endif()

set(TOMATO_INCLUDES ${tomato_INCLUDES} CACHE STRING "TomatoLib include directories")
set(TOMATO_LIBRARIES ${tomato_LIBRARIES} CACHE STRING "TomatoLib dependencies")
