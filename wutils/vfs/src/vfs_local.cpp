#include "vfs.h"

#include <fstream>
#include <cstdarg>

// DEFINITION LocalFS


LocalFS::LocalFS(fs::path root, VFileEx& ex) {
    if(!fs::exists(root)) {
        ex.code = VFS_FOLDER_DONT_EXISTS;
        fill_ex_bad_type(ex,root.string());
        return;
    }
    _root = root;
}

fs::path LocalFS::stripRoot(const fs::path& path) {

}

VPath LocalFS::toVPath(const fs::path& path) {
    VPath p;
    for(const auto& i : path) {
        p.append(i.string());
    }
    return p;
}

VFileInfo LocalFS::GetFileInfo(const VPath& path,VFileEx& ex) {
	if(path.is_directory()) {
		fill_ex_bad_type(ex,toString(path));
		return {};
	}
    return VFileInfo();
}

std::vector<VFileInfo> LocalFS::GetFilesInfoDirectory(const VPath& path,VFileEx& ex) {
    fs::path p = _root / toString(path);
    std::vector<VFileInfo> files;
    std::error_code ec;
    for(const auto& i : fs::directory_iterator(p)) {
        if(fs::is_regular_file(i,ec)) {
            VPath vp = toVPath(i.path());
            std::error_code e;
            int size = fs::file_size(i.path(),e);
            files.emplace_back(VFileInfo(vp,size));
        }
    }
    return files;
}

VData LocalFS::ReadFile(const VFileInfo& file,VFileEx& ex) {
    fs::path p = _root / toString(file.getPath());
    if(!fs::exists(p)) {
        ex.code = VFS_FILE_DONT_EXISTS;
        fill_ex_bad_type(ex,p.string());
        return {};
    }
    std::ifstream ifs(p,std::ios::binary);
    if(!ifs.is_open()) {
        ex.code = VFS_READING_ERROR;
        fill_ex_bad_type(ex,p.string());
        return {};
    }
    VData d;
    d.size = ifs.tellg();
    d.data = std::make_unique<char[]>(d.size);
    ifs.read(d.data.get(),d.size);
    ifs.close();
}

std::future<VData> LocalFS::GetFileDataAsync(const VFileInfo& p,VFileEx& ex) {

    return std::async(std::launch::async,[&]()->VData{
        return ReadFile(p,ex);
    });
}

bool LocalFS::Exists(const VPath& p) {
    return fs::exists(_root);
}

void LocalFS::CreateDirectory(const VPath& p,VFileEx& ex) {

}

void LocalFS::CreateFile(const VPath& p,VFileEx& ex) {

}

void LocalFS::WriteFile(const VPath& p,VFS_IOS ios,VFileEx& ex) {

}

std::string LocalFS::toString(const VPath& path) {
    fs::path p;
    for(const auto& i : path.name) {
        p = p / i;
    }
    return p.string();
}