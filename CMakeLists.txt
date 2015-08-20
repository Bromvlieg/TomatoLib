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

set(TOMATO_INCLUDES "" PARENT_SCOPE)
list(APPEND TOMATO_INCLUDES ${GLEW_INCLUDE_DIRS})
list(APPEND TOMATO_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/TomatoLib/TomatoLib)
list(APPEND TOMATO_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/TomatoLib/TomatoLib/Libaries/glfw/include)
list(APPEND TOMATO_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/TomatoLib/TomatoLib/Libaries/freetype-gl)
list(APPEND TOMATO_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/TomatoLib/TomatoLib/Libaries/VTFLib/src)
list(APPEND TOMATO_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/TomatoLib/TomatoLib/Libaries/VTFLib/src)

set(TOMATO_LIBRARIES "" PARENT_SCOPE)
list(APPEND TOMATO_LIBRARIES ${GLEW_LIBRARIES})
list(APPEND TOMATO_LIBRARIES glfw)
list(APPEND TOMATO_LIBRARIES ${GLFW_LIBRARIES})
list(APPEND TOMATO_LIBRARIES VTFLib13)

include_directories(${TOMATO_INCLUDES})
target_link_libraries(tomato
	${TOMATO_LIBRARIES}
	stdc++
)
