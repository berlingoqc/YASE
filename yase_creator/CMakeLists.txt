
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/default_shaders  DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

include_directories ("${PROJECT_SOURCE_DIR}/imgui")
include_directories ("${PROJECT_SOURCE_DIR}/crazygl")
include_directories ("${PROJECT_SOURCE_DIR}/wutils")
include_directories ("${PROJECT_SOURCE_DIR}/yase/include")

add_executable(yase_creator env_page.h main.cpp home.h)

target_link_libraries(yase_creator
	imgui
	yase
	crazygl
	wutils
	${OPENGL_LIBRARY}
	${GL_LIBS}
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	add_definitions(-DEMSCRIPTEN)
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set_target_properties(yase_creator PROPERTIES LINK_FLAGS "--emrun   -s WASM=1 -std=c++11 -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s FETCH=1 --preload-file default_shaders")
endif ()