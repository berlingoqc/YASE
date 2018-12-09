
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/default_shaders  ${CMAKE_CURRENT_SOURCE_DIR}/model DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

include_directories ("${PROJECT_SOURCE_DIR}/crazygl")
include_directories ("${PROJECT_SOURCE_DIR}/imgui")
include_directories ("${PROJECT_SOURCE_DIR}/YASE")

add_executable(yase_creator env_page.h main.cpp home.h)

target_link_libraries(yase_creator
	crazygl
	imgui
	soil
	${OPENGL_LIBRARY}
	${GL_LIBS}
)
if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	## --preload-file textures --preload-file shaders --preload-file fonts
	add_definitions(-DEMSCRIPTEN -D_LITTLE_ENDIAN)
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set_target_properties(yase_creator PROPERTIES LINK_FLAGS "--emrun -s WASM=1 -std=c++11 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 --use-preload-plugins --preload-file default_shaders --preload-file model")
endif ()