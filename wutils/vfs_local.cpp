#include "vfs.h"

#include <fstream>
#include <cstdarg>

// DEFINITION LPath

LPath::LPath(fs::path p) : _path(p) {
}

std::string LPath::toString() const {
    return _path.string();
}

void LPath::append(std::string p) {
    _path = _path / p;
}


void LPath::append(const VPath& p) {
    append(p.toString());
}


// DEFINITION LocalFS


LocalFS::LocalFS(fs::path root, VFileEx& ex) {
    if(!fs::exists(root)) {
        ex.code = VFS_FOLDER_DONT_EXISTS;
        fill_ex_bad_type(ex,root.string());
        return;
    }
    _root = root;
}

VPath LocalFS::GetVPath(int items,...) {
	VPath p;
	va_list arguments;
	va_start(arguments,items);
	for(int i=0;i<items;i++) {
		p.append(va_arg(arguments,const char*));
	}
	va_end(arguments);
	return p;
}

VFileInfo LocalFS::GetFileInfo(const VPath& path,VFileEx& ex) {
	if(path.is_directory()) {
		fill_ex_bad_type(ex,path.toString());
		return {};
	}
}

std::vector<VFileInfo> LocalFS::GetFilesInfoDirectory(const VPath& path,VFileEx& ex) {
    fs::path p = _root / path.toString();
    std::vector<VFileInfo> files;
    for(auto& p : fs::directory_iterator(p)) {
        if(fs::is_regular_file(p)) {
            VFileInfo vf;
            vf.path = LPath(p.path());
            std::error_code e;
            vf.size = fs::file_size(p.path(),e);
            files.emplace_back(vf);
        }
    }
    return files;
}

VData LocalFS::ReadFile(const VFileInfo& file,VFileEx& ex) {
    fs::path p = _root / file.getPath().toString();
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