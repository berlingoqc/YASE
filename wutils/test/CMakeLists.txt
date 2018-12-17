#include_directories("${PROJECT_SOURCE_DIR}/wutils/")
add_executable(test_vfs local_fs.cpp)
target_link_libraries(test_vfs wutils Catch2::Catch2)


include(CTest)
include(Catch)
catch_discover_tests(test_vfs)