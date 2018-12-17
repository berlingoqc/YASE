#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include <string>
#include <vector>
#include <exception>
#include <future>

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

enum VFS_ERROR_CODE {
	VFS_NO_ERROR,
    VFS_FILE_DONT_EXISTS,
    VFS_FOLDER_DONT_EXISTS,
	VFS_READING_ERROR,
	VFS_WRITTING_ERROR,
	VFS_NOT_DIR_ERROR,
	VFS_NOT_FILE_ERROR
};

enum VFS_FILE_TYPE {
	VFS_FOLDER,
	VFS_FILE
};

enum VFS_IOS {
	VFS_BINARY = 1 << 0,
	VFS_STRING = 1 << 1,
	VFS_TRUNCATE = 1 << 2,
	VFS_APPEND = 1 << 3
};


enum VFS_FILE_PERMISSION {
	VFS_PERMISSION_READ = 1 << 0,
	VFS_PERMISSION_WRITE = 1 << 1
};


template<typename T>
class VList {
protected:
	std::unique_ptr<T[]>	_container;
	int 					_size;

	VList<T>(int size) : _size(size),_container(std::make_unique<T[]>(size)) {
	}	
};


struct VFileEx : public std::exception {
	VFS_ERROR_CODE 			code;
	std::string 			message;


	VFileEx() : code(VFS_NO_ERROR), message("") {
	}

	VFileEx(VFS_ERROR_CODE e, std::string n) {
		code = e;
		message = n;
	}

	const char* what() const throw() {
		return message.c_str();
	}
};


inline void fill_ex_bad_type(VFileEx& ex, const std::string& name) {
	switch(ex.code) {
		case VFS_FILE_DONT_EXISTS: ex.message = "Le fichier n'existe pas " + name; break;
		case VFS_FOLDER_DONT_EXISTS: ex.message = "Le dossier n'existe pas " + name; break;
		case VFS_NOT_DIR_ERROR: ex.message = name + " n'est pas un dossier"; break;
		case VFS_NOT_FILE_ERROR: ex.message = name + " n'est pas un fichier"; break;
		default: ex.message = "Erreur durant une operation sur le fichier "+name; break;
	} 
}

struct VData {
	std::unique_ptr<char[]>		data;
	int							size;
};

struct VDirectory;


// VPath est toujours relative a son parent
struct VPath {
	std::shared_ptr<VDirectory>			parent;
	std::string							name;
	VFS_FILE_TYPE						type;

	bool	is_directory() const {
		return type == VFS_FOLDER;
	}

	bool	is_file() const {
		return type == VFS_FILE;
	}

	// Append au path courant , est valider la prochaine fois que passer a VFS
	void	append(std::string p);
	void	append(const VPath& p);


	std::string toString() const;

};

struct VFileInfo {

	int			size;
	VPath		path;

	const VPath&	getPath() const {
		return path;
	}

	int	getSize() const {
		return size;
	}

};

class VDirectoryInfo : public VList<VFileInfo> {

public:
	VDirectoryInfo(int size) : VList<VFileInfo>(size) {

	}

	int	GetTotalSize() const {

		return 0;
	}

	const VFileInfo&	GetFileInfo(int index);
	const VFileInfo&	GetFileInfo(VPath path);
};

class VFS {

public:
	// Retourne une struture d'information sur un fichier
	virtual VFileInfo					GetFileInfo(const VPath& path,VFileEx& ex) = 0;
	// Retourne la liste des fichiers dans un repertoire
	virtual std::vector<VFileInfo>		GetFilesInfoDirectory(const VPath& path,VFileEx& ex) = 0;
	// Retourne le contenue d'un fichier de facon synchrone
	virtual VData						ReadFile(const VFileInfo& file,VFileEx& ex) = 0;
	// Retourne le contenue d'une fichier de facon synchrone
	virtual std::future<VData> 			GetFileDataAsync(const VFileInfo& p,VFileEx& ex) = 0;
	// Indique si le vpath exists peux importer c'est quelle type
	virtual bool Exists(const VPath& p) = 0;
	// Crée un nouveau repertoire dans le path indiquer
	virtual void CreateDirectory(const VPath& p,VFileEx& ex) = 0;
	// Crée un nouveau fichier vide
	virtual void CreateFile(const VPath& p,VFileEx& ex) = 0;
	// Écrit dans un fichier avec le payload passer de facon synchrone
	virtual void WriteFile(const VPath& p,VFS_IOS ios,VFileEx& ex) = 0;
};
// VPath est toujours relative a son parent
struct LPath : VPath {

	fs::path	_path;

	LPath(fs::path p);

	void	append(std::string p);
	void	append(const VPath& p);

	std::string toString() const;
};

class LocalFS : public VFS {
	fs::path		_root;


public:

	LocalFS(fs::path root,VFileEx& ex);

	VFileInfo GetFileInfo(const VPath& path,VFileEx& ex) override;
	std::vector<VFileInfo>		GetFilesInfoDirectory(const VPath& path,VFileEx& ex) override;
	VData						ReadFile(const VFileInfo& file,VFileEx& ex) override;
	std::future<VData> 			GetFileDataAsync(const VFileInfo& p,VFileEx& ex) override;
	bool Exists(const VPath& p) override;
	void CreateDirectory(const VPath& p,VFileEx& ex) override;
	void CreateFile(const VPath& p,VFileEx& ex) override;
	void WriteFile(const VPath& p,VFS_IOS ios,VFileEx& ex) override;
};

#endif