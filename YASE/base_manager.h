#ifndef BASE_MANAGER_H
#define BASE_MANAGER_H

#include <filesystem>
#include <iostream>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

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