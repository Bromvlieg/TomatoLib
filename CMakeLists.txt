cmake_minimum_required(VERSION 3.0)
project(TomatoLib)

cmake_policy(VERSION 3.0)

set(tomato_INCLUDES "")
set(tomato_LIBRARIES "")

add_subdirectory("TomatoLib/TomatoLib/Libaries")

list(APPEND tomato_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/TomatoLib/TomatoLib)

set(presrc "TomatoLib/TomatoLib")

find_package(PkgConfig REQUIRED)
pkg_search_module(GLEW REQUIRED glew)
pkg_search_module(FT2 REQUIRED freetype2)

list(APPEND tomato_INCLUDES ${FT2_INCLUDE_DIRS})
list(APPEND tomato_INCLUDES ${GLEW_INCLUDE_DIRS})

list(APPEND tomato_LIBRARIES ${GLEW_LIBRARIES})
list(APPEND tomato_LIBRARIES ${FT2_LIBRARIES})

add_library(tomato
	${presrc}/MapFormats/Bsp.cpp
	${presrc}/Networking/EzSock.cpp
	${presrc}/Networking/Packet.cpp
	${presrc}/Networking/Connection.cpp
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
