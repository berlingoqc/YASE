# CMakeList.txt : CMake project for YASE, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_library (YASE "YASE.cpp" "YASE.h" "environment.h")

# TODO: Add tests and install targets if needed.
