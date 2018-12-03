# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("YASE")

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

set(GLEW_INCLUDE_PATH "" CACHE PATH "The directory that contains GL/glew.h" )
set(GLEW_LIBRARY "" CACHE FILEPATH "libglew.a or glew.lib" )

set(GLM_INCLUDE_PATH "" CACHE PATH "The directory that contains glm/glm.hpp" )

set(GLFW_INCLUDE_PATH "" CACHE PATH "The directory that contains GLFW/glfw3.h" )
set(GLFW_LIBRARY "" CACHE PATH "The directory that contains glfw3.lib" )

set(SOIL_INCLUDE_PATH "" CACHE PATH "The directory that contains SOIL.h" )
set(SOIL_LIBRARY "" CACHE PATH "The directory that contains SOIL.lib" )

set(ASSIMP_INCLUDE_PATH "" CACHE PATH "The directory that contains assimp/assimp.h" )
set(ASSIMP_LIBRARY "" CACHE PATH "The directory that contains assimp.lib" )

set(STB_INCLUDE_PATH "" CACHE PATH "The directory that contains stb_image.h")

set(FB_INCLUDE_PATH "" CACHE PATH "The directory that contains flatbuffers/flatbuffers.h" )
set(FB_LIBRARY "" CACHE PATH "The directory that contains flatbuffers.lib" )

include (FindProtobuf)
Find_Package (Protobuf REQUIRED)

set( GL_LIBS glew32d glfw3 SOIL assimp-vc140-mt libprotobufd libprotocd)
if(win32)
	set( GL_LIBS GL GLEW glfw SOIL stdc++fs ${PROTOBUF_LIBRARY})
endif()



include_directories(
	${GLEW_INCLUDE_PATH}
	${GLFW_INCLUDE_PATH}
	${GLM_INCLUDE_PATH}
	${SOIL_INCLUDE_PATH}
	${ASSIMP_INCLUDE_PATH}
	${STB_INCLUDE_PATH}
	${FB_INCLUDE_PATH}
	${PROTOBUF_INCLUDE_DIR}
	fb_def/include
)

link_directories(
	${GLEW_LIBRARY}
	${Protobuf_LIBRARIES}
	${ASSIMP_LIBRARY}
	${GLFW_LIBRARY}
	${FB_LIBRARY}
	${SOIL_LIBRARY}
)
add_definitions (-DPROTOBUF_USE_DLLS)

# Include sub-projects.
add_subdirectory ("proto_def")
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/proto_def")

add_subdirectory ("imgui")
add_subdirectory ("crazygl")
add_subdirectory ("yase_creator")
