#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include <string>
#include <vector>
#include <exception>
#include <future>


#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

enum VFS_ERROR_CODE {
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

enum VFS_FILE_PERMISSION {

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
	std::unique_ptr<char*>		data;
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

	// Append une partie au path ( comme un fichier ou un repertoire )
	// ne valide pas tout de suite que le path est valide c'est quand
	// on le donne a VFS
	void	append(std::string p);
	// Ajoute un autre path au bout de mon path
	void	append(const VPath& p);

	// Retourn une version string de chemin
	std::string toString() const;

};

struct VFileInfo {

	int			size;

public:

	VFileInfo();

	const VPath&	getPath();

};

class VDirectoryInfo : public VList<VFileInfo> {

public:
	VDirectoryInfo(int size) : VList<VFileInfo>(size) {

	}

	int	GetTotalSize() {

		return 0;
	}

	const VFileInfo&	GetFileInfo(int index);
	const VFileInfo&	GetFileInfo(VPath path);

};

class VFS {

public:

	// Get un VPath construit avec les differences morceaux passé
	// toujours a partir du root folder de VFS
	virtual VPath						GetVPath(std::string items,...) = 0;


	// Retourne une structure d'informations sur un fichier
	virtual VFileInfo					GetFileInfo(const VPath& path) = 0;

	// Retourn l'information sur tout le fichiers du repertoire
	virtual VDirectoryInfo				GetFilesInfoDirectory(const VPath& path) = 0;


	// Lit le contenue d'un fichier de facon synchrone
	virtual VData						ReadFile(const VFileInfo& file,VFileEx& ex);
	
	// Lit le contenue d'un fichier de facon asynchrone
	// les donnes sont contenue dans le future retourner
	virtual std::future<VData> 			GetFileDataAsync(const VFileInfo& p,VFileEx& ex);

	// Indique si le path exists peux importe il s'agit de quoit
	virtual bool Exists(const VPath& p);

	// Crée un nouveau repertoire de VPath relative au root folder
	virtual bool CreateDirectory(const VPath& p);
};

class LocalFS : public VFS {


	fs::path 		root_directory;


public:

	LocalFS(std::string root_path) {

	}


	VPath GetVPath(int items,...) {
		VPath p;
		va_list arguments;
		va_start(arguments,items);
		for(int i=0;i<items;i++) {
			p.append(va_arg(arguments,const char*));
		}
		va_end(arguments);
		return p;
	}

	VFileInfo GetFileInfo(const VPath& path,VFileEx& ex) {
		if(!Exists(path)) { // Erreur si le path n'existe pas
			ex.code = VFS_FILE_DONT_EXISTS;
			fill_ex_bad_type(ex,path.toString());
			return {};
		}
		if(path.is_directory()) {
			ex.code = VFS_NOT_FILE_ERROR;
			fill_ex_bad_type(ex,path.toString());
			return {};
		}
		// Va chercher l'information sur le dossier	
	}
};

#endif