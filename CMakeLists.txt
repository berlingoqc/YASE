# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("YASE")

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

set(GLEW_INCLUDE_PATH "" CACHE PATH "The directory that contains GL/glew.h" )
set(GLEW_LIBRARY "" CACHE PATH "libglew.a or glew.lib" )

set(GLM_INCLUDE_PATH "" CACHE PATH "The directory that contains glm/glm.hpp" )

set(GLFW_INCLUDE_PATH "" CACHE PATH "The directory that contains GLFW/glfw3.h" )
set(GLFW_LIBRARY "" CACHE PATH "The directory that contains glfw3.lib" )

set(SOIL_INCLUDE_PATH "" CACHE PATH "The directory that contains SOIL.h" )
set(SOIL_LIBRARY "" CACHE PATH "The directory that contains SOIL.lib" )

set(ASSIMP_INCLUDE_PATH "" CACHE PATH "The directory that contains assimp/assimp.h" )
set(ASSIMP_LIBRARY "" CACHE PATH "The directory that contains assimp.lib" )

set(YAS_INCLUDE_PATH "" CACHE PATH "The directory that contains yas/serialize.hpp")


if(UNIX)
	set( BASE_LIBS stdc++fs )
	set( GL_LIBS GL GLEW glfw ${BASE_LIBS} assimp SOIL)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	add_subdirectory("3dparty/soil")
	set( GL_LIBS GL GLEW glfw libassimp soil)
else() #windows
	set( GL_LIBS glew32d glfw3 SOIL assimp-vc140-mt)
endif()

include_directories(
	${GLEW_INCLUDE_PATH}
	${GLFW_INCLUDE_PATH}
	${GLM_INCLUDE_PATH}
	${SOIL_INCLUDE_PATH}
	${ASSIMP_INCLUDE_PATH}
	${YAS_INCLUDE_PATH}
)

link_directories(
	${GLEW_LIBRARY}
	${ASSIMP_LIBRARY}
	${GLFW_LIBRARY}
	${SOIL_LIBRARY}
)



# Include sub-projects.
add_subdirectory ("imgui")
add_subdirectory ("crazygl")
add_subdirectory ("asset_converter")
#add_subdirectory ("wutils")
add_subdirectory ("yase")
add_subdirectory ("yase_creator")