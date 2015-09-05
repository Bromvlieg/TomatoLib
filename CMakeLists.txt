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
	set(GLEW_INCLUDE_DIRS "GLEW_INCLUDE_DIRS_NOTFOUNd" CACHE STRING "Glew include dirs")

	set(FT2_LIBRARIES "FT2_LIBRARIES_NOTFOUND" CACHE STRING "Freetype 2 libraries")
	set(FT2_INCLUDE_DIRS "FT2_INCLUDE_DIRS_NOTFOUND" CACHE STRING "Freetype 2 include dirs")

	message(FATAL_ERROR "PkgConfig not found, so you have to provide paths for FT2 and GLEW!")
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
	${presrc}/Math/Vector3.cpp
	${presrc}/Math/Vector2.cpp
	${presrc}/ModelFormats/Obj.cpp
	${presrc}/Utilities/Utilities.cpp
	${presrc}/Utilities/HTTP.cpp
	${presrc}/Game/Game.cpp
	${presrc}/Game/Camera.cpp
	${presrc}/Game/Window.cpp
	${presrc}/Libaries/LodePNG/lodepng_util.cpp
	${presrc}/Libaries/LodePNG/lodepng.cpp
	${presrc}/Libaries/md5/md5.cpp
)

add_definitions(-DLINUX -std=c++11)

message(${tomato_INCLUDES})

include_directories(${tomato_INCLUDES})
target_link_libraries(tomato
	${tomato_LIBRARIES}
	stdc++
)

set(TOMATO_INCLUDES ${tomato_INCLUDES} CACHE STRING "TomatoLib include directories")
set(TOMATO_LIBRARIES ${tomato_LIBRARIES} CACHE STRING "TomatoLib dependencies")
