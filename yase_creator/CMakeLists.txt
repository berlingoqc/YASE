
file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/default_shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

include_directories ("${PROJECT_SOURCE_DIR}/crazygl")
include_directories ("${PROJECT_SOURCE_DIR}/imgui")
include_directories ("${PROJECT_SOURCE_DIR}/YASE")

add_executable(yase_creator env_page.h main.cpp home.h)

target_link_libraries(yase_creator
	crazygl
	imgui
	proto
	${OPENGL_LIBRARY}
	${GL_LIBS}
)