#ifndef BASE_MANAGER_H
#define BASE_MANAGER_H

#include <iostream>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "model.h"
#include <yas/serialize.hpp>
#include <yas/std_types.hpp>
#include <yas/file_streams.hpp>

constexpr  size_t flags_yas_bf = yas::file | yas::binary;
struct FileDontExist : public std::exception {

	string	file;

	const char* what() const throw() {
		return ("Le fichier " + file + " n'exsite pas").c_str();
	}
};


class AssetManager
{
protected:
	fs::path		root_folder;
	std::string		file_name;

private:

	void ValidFile(const fs::path& fn) {
		if (root_folder.empty()) {
			FileDontExist e;
			e.file = fn.string();
			throw e;
		}
	}

public:
	virtual void saveManager(ostream* writer) { }
	virtual void loadManager(ifstream* reader) { }

	void Save() {
		const fs::path fn = getFileFullPath();
		ValidFile(fn);
		ofstream of(getFileFullPath(), ios::binary);
		saveManager(&of);
		of.close();
	}

	void Load() {
		const fs::path fn = getFileFullPath();
		ValidFile(fn);
		ifstream inf(fn.string(), std::ios::binary | std::ios::in);
		loadManager(&inf);
		inf.close();
	}

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