include_directories ("${PROJECT_SOURCE_DIR}/crazygl")
include_directories ("${PROJECT_SOURCE_DIR}/wutils")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_definitions(-D_LITTLE_ENDIAN)

add_library(yase animation_manager.cpp base_manager.cpp camera.cpp env_manager.cpp model_manager.cpp scene_manager.cpp shader_manager.cpp skybox_manager.cpp sound_manager.cpp texture_manager.cpp)

link_libraries(yase crazygl)