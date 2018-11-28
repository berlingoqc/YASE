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


if(win32)
	set( GL_LIBS glew32 glfw3 glad SOIL assimp-vc140-mt )
else(win32)
	set( GL_LIBS GL glut GLU GLEW stdc++fs )
endif(win32)


set( GL_LIBS glew32d glfw3 SOIL assimp-vc140-mt flatbuffers )

include_directories(
	${GLEW_INCLUDE_PATH}
	${GLFW_INCLUDE_PATH}
	${GLM_INCLUDE_PATH}
	${SOIL_INCLUDE_PATH}
	${ASSIMP_INCLUDE_PATH}
	${STB_INCLUDE_PATH}
	${FB_INCLUDE_PATH}
	fb_def/include
)

link_directories(
	${GLEW_LIBRARY}
	${ASSIMP_LIBRARY}
	${GLFW_LIBRARY}
	${FB_LIBRARY}
	${SOIL_LIBRARY}
)


# Include sub-projects.
add_subdirectory ("imgui")
add_subdirectory ("crazygl")
add_subdirectory ("YASE")
add_subdirectory ("yase_creator")
