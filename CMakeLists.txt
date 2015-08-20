cmake_minimum_required(VERSION 3.0)
project(TomatoLib)

add_subdirectory("TomatoLib/TomatoLib/Libaries")

set(presrc "TomatoLib/TomatoLib")

find_package(PkgConfig REQUIRED)
pkg_search_module(GLEW REQUIRED glew)

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
	${presrc}/Math/Matrix.cpp
	${presrc}/Math/AABB.cpp
	${presrc}/Math/Vector3.cpp
	${presrc}/Math/Vector2.cpp
	${presrc}/Utilities/Utilities.cpp
	${presrc}/Utilities/HTTP.cpp
	${presrc}/Game/Game.cpp
	${presrc}/Game/Camera.cpp
	${presrc}/Game/Window.cpp
	${presrc}/Libaries/LodePNG/lodepng_util.cpp
	${presrc}/Libaries/LodePNG/lodepng.cpp
)

add_definitions(-DLINUX -std=c++11)

set(tomato_INCLUDES "")
list(APPEND tomato_INCLUDES ${GLEW_INCLUDE_DIRS})
list(APPEND tomato_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/TomatoLib/TomatoLib)
list(APPEND tomato_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/TomatoLib/TomatoLib/Libaries/glfw/include)
list(APPEND tomato_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/TomatoLib/TomatoLib/Libaries/freetype-gl)
list(APPEND tomato_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/TomatoLib/TomatoLib/Libaries/VTFLib/src)
list(APPEND tomato_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/TomatoLib/TomatoLib/Libaries/VTFLib/src)

set(tomato_LIBRARIES "")
list(APPEND tomato_LIBRARIES ${GLEW_LIBRARIES})
list(APPEND tomato_LIBRARIES glfw)
list(APPEND tomato_LIBRARIES ${GLFW_LIBRARIES})
list(APPEND tomato_LIBRARIES VTFLib13)

include_directories(${tomato_INCLUDES})
target_link_libraries(tomato
	${tomato_LIBRARIES}
	stdc++
)

set(TOMATO_INCLUDES ${tomato_INCLUDES} CACHE STRING "TomatoLib include directories")
set(TOMATO_LIBRARIES ${tomato_LIBRARIES} CACHE STRING "TomatoLib dependencies")
