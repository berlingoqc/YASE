#ifndef BASE_MANAGER_H
#define BASE_MANAGER_H

#include "header.h"

#include <map>

#include <yas/serialize.hpp>
#include <yas/std_types.hpp>
#include <yas/file_streams.hpp>

#include "model.h"
#include "vfs.h"

using namespace yas;

constexpr  std::size_t flags_yas_bf = yas::file | yas::binary;

struct FileDontExist : public std::exception {

	std::string	file;

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

	void ValidFile(const fs::path& fn);

public:
	virtual void saveManager(std::ostream* writer) {};
	virtual void loadManager(std::ifstream* reader) { };

	void Save();

	void Load();

	void setRootFolder(fs::path rf);

	fs::path getFileFullPath() const;

};


#endif