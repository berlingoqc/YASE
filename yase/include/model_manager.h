#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "logger.h"
#include "textures.h"

#include "base_manager.h"
#include "texture_manager.h"

using namespace Assimp;


static glm::vec3	vertex_base[4]
{
	{-500,0,-500}, {500,0,-500}, {-500,0,500}, {500,0,500}
};
static unsigned int	indice_base[6]
{
	0,1,2,1,2,3
};
static glm::vec2	tex_bas[4]
{
	{0.0,0.0}, {100,0}, {0,100}, {100,100}
};



class YaseMesh
{
public:
	bool								have_ibo = true;
	bool								have_tangant = false;
	bool								have_bitangant = false;

	uint								VAO = 0 , VBO = 0 , EBO= 0 ;

	std::vector<Vertex>		vertices;
	std::vector<uint>					indices;
	std::vector<uint>					textures;

	std::vector<std::string>					needed_texture;

	YaseMesh();
	YaseMesh(std::vector<Vertex> vert, std::vector<uint> indices, std::vector<std::string> texture);

	template<typename Ar>
	void serialize(Ar& ar) {
		ar & YAS_OBJECT(nullptr, vertices, indices, needed_texture);
	}


	const std::vector<std::string>& getNeededTexture() const { return needed_texture; }

	void setTextures(std::vector<uint> text)
	{
		textures = text;
	}

	void Allocate();


	void Draw(uint shader);

};

class YaseModel
{
public:
	std::string					name = "";
	bool					has_been_update = false;

	std::vector<YaseMesh>	meshes;

	fs::path				directory = "";

	template<typename Ar>
	void serialize(Ar& ar) {
		ar & YAS_OBJECT(nullptr, name, meshes);
	}

	const std::vector<YaseMesh>& getMeshes() { return meshes; }
	bool hasBeenUpdate() { return has_been_update; }

	// Draw dessine le model avec le shader donner
	void Draw(uint shader);

	bool ReadModelInfoAssimp(fs::path filename,TextureManager* tm);

	void processNode(aiNode* node, const aiScene* scene,TextureManager* tm);

	YaseMesh processMesh(aiMesh* mesh, const aiScene* scene,TextureManager* tm);

	// Cr�e les buffer gpu et charge les textures requis depuis le texturemanager
	void LoadModel(TextureManager* tm);
	
	std::vector<std::string> loadMaterialTextures(aiMaterial* mat,aiTextureType type, std::string typeName,TextureManager* tm);

};

struct ModelException : public std::exception {

	bool exists;
	std::string name;

	ModelException(bool e, std::string n) {
		exists = e;
		name = n;
	}

	const char* what() const throw() {
		if (exists)
			return (("Le model exists deja " + name).c_str());
		return (("Le model n'existe pas " + name).c_str());
	}
};


class ModelManager : public AssetManager
{
	inline const static std::string		extension_model = ".model";
	inline const static std::string		mesh_folder[2]{ "yase", "obj" };


	// Une map qui contient tous mes models que je possede
	// dans mon arborscence. La liste est loader au demarrage
	// et les assets du model sont loader quand on n'a besoin
	// et les textures sont entreposer dans TextureManager
	std::map<std::string, YaseModel*>		map_model;
	std::vector<std::string>						keys;
	int									index_loaded_model = 0;


public:
	TextureManager*					tex_manager = nullptr;
	std::vector<const char*> getKeys()
	{
		std::vector<const char*> v;
		for (const auto& t : map_model)
			v.emplace_back(t.first.c_str());
		return v;
	}

	template<typename Ar>
	void serialize(Ar& ar) {
		ar & YAS_OBJECT(nullptr, keys);
	}

	void generateMap();

	virtual void saveManager(std::ostream* writer) {

	}

	virtual void loadManager(std::ifstream* reader) {

	}

	ModelManager();

	const std::map<std::string, YaseModel*>&	getModelMap() {
		return map_model;
	}

	int getNumberModel() {
		return map_model.size();
	}

	int getNumberLoadedModel() {
		return index_loaded_model;
	}


	// Valide que tous les models present dans le fichier exists dans le repertoire
	void ValidIntegrity();

	void ImportModel(std::string name, fs::path filepath);
	// Charge depuis la memoire les informations du model (meshes)
	void ReadModelInfo(std::string key, fs::path fullpath,YaseModel* ym);

	// Import un model d'un autre type de fichier dans mon mode de fichier avec ce qu'assimp supporte
	void AddModelAssimp(std::string name, fs::path filepath);

	void AddModel(std::string name, YaseModel ym);

	// Ajout un yasemodel existant dans la collection. Le sauvegarde tout de suite. C'est la fonction qui s'occupe de ca
	void AddModel(std::string name, Model model);
	// Retourne le pointeur d'un model deja loader. ( Le load si besoin )
	YaseModel*	getLoadedModel(std::string name);
	// Efface le model de la memoire de gpu ( quand on n'a plus besoin , faudrait je garde le nombre de reference )
	void releaseModel(std::string name);


};


#endif