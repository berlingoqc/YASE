
#define CATCH_CONFIG_MAIN


#include <catch2/catch.hpp>
#include "vfs.h"

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}


bool TestLocalVFS(std::string root_path) {
    printf("Test LocalFS\n");

    VFileEx ex;
    auto p = fs::path(root_path);
    LocalFS localfs(p,ex);
    if(ex.code != VFS_NO_ERROR) {
        printf("VFileEx code %d message %s\n",ex.code,ex.message.c_str());
        return false;
    }
    VPath vp;
    vp.append("");
    for( const auto& i : localfs.GetFilesInfoDirectory(vp,ex)) {
        printf("Info file %s size %d\n",localfs.toString(i.getPath()).c_str(),i.getSize());
    }
    return true;
}




TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE(TestLocalVFS("/home/wq/Project/YASE/"));
}