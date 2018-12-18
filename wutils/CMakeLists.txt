cmake_minimum_required(VERSION 3.8)

project("wutils")

set(CMAKE_CXX_STANDARD 17)

if(UNIX)
    set( NEED_LIBS stdc++fs pthread )
endif()



find_package(Catch2 REQUIRED)

enable_testing()

add_subdirectory(vfs)
