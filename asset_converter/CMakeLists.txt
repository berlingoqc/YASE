cmake_minimum_required(VERSION 3.8)

find_package(OpenCV REQUIRED)

include_directories(
    "${PROJECT_SOURCE_DIR}/yase"
    "${OpenCV_INCLUDE_DIRS}"
)

add_executable(asset_converter main.cpp) 

target_link_libraries( asset_converter yase ${OpenCV_LIBS} ${BASE_LIBS})