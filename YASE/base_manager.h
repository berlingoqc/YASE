#ifndef BASE_MANAGER_H
#define BASE_MANAGER_H

#include <filesystem>
#include <iostream>
#include <string>
#include <map>
#include <iostream>
#include <fstream>


namespace fs = std::filesystem;
using namespace std;

struct RootFolderNotConfigure : public std::exception {

	string	asset_manager_name;

	const char* what() const throw() {
		return ("Le repertoire de " + asset_manager_name + " n'est pas configurer").c_str();
	}
};


class AssetManager
{
protected:
	fs::path		root_folder;
	std::string		file_name;



public:
	void setRootFolder(fs::path rf)
	{
		root_folder = rf;
	}

	fs::path getFileFullPath() const
	{
		return (root_folder / file_name);
	}

};


#endif