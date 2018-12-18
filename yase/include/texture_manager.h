#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <iostream>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "logger.h"
#include "textures.h"

#include "base_manager.h"

#include <yas/serialize.hpp>
#include <yas/std_types.hpp>

#include "model.h"


struct YaseTextureInfo {
	Texture			texture; // Contient l'informations de la texture
	TextureOption	option; // Contient les informations de chargement de la texture
	uint						gl_id = ERROR_TEXTURE; // Contient l'id opengl de la texture si elle a ete loader

	template<typename Ar>
	void serialize(Ar& ar) {
		ar & YAS_OBJECT(nullptr, texture, option);
	}
};

// TextureManager s'occupe de gerrer nos textures et de loader
// ceux qu'on veut utiliser quand le temps est venu
class TextureManager : public AssetManager {
public:
	ENGINE::MyTexture							texture_loader; // Classe pour charger les textures
	std::vector<Texture>						textures; // Liste de nos textures dans notre repertoire
	std::vector<std::string>					categories; // Liste des cat�gories de texture ??
	std::map<std::string, YaseTextureInfo>		loaded_textures; // Contient les informations sur des textures avec les info de loading utiliser par un context donner

	int nbr_loaded_gl = 0;


	template<typename Ar>
	void serialize(Ar& ar)
	{
		ar & YAS_OBJECT(nullptr,textures,categories,loaded_textures);
	}

	TextureManager();

	std::vector<Texture>&	getTextures()
	{
		return textures;
	}

	// Retourne la map des textures qui ont besoin d'etre loader ( avec leur settings )
	const std::map<std::string, YaseTextureInfo>& getLoadedtextures() {
		return loaded_textures;
	}


	// Retourne les informations d'une texture loader ( dependence d'un model )
	YaseTextureInfo* getLoadedTextures(std::string name) {
		if (const auto& i = loaded_textures.find(name); i != loaded_textures.end()) {
			return &i->second;
		}
		return nullptr;
	}


	// Retourne la listes des categories existantes
	std::vector<std::string> getCategories() const {
		return categories;
	}

	// Retourne le nombre de fichier de texture existant
	int getTexturesCount() const {
		return textures.size();
	}

	// Retounr le nombre de texture avec configuration existant
	int getTexturesLoadedCount() const {
		return loaded_textures.size();
	}

	// Retourne le nombre de texture charg� dans opengl ( qui ont un handler)
	int getGltextureCount() const {
		return nbr_loaded_gl;
	}

	uint getTexGLID(std::string name);

	void getTexGLID(YaseTextureInfo* t);

	std::vector<uint> loadTexture(std::vector<std::string> keys);

	YaseTextureInfo* loadTexture(std::string key);

	void AddNewLoadedTexture(std::string name, std::string tex_name, TexWrappingOptions w, TexFilterOptions f);


	bool AddNewTexture(std::string filename,char* data, int size,std::string category = "default");

	// AddNewTexture ajoute une nouvelle texture dans
	// la liste et cree son information
	bool AddNewTexture(fs::path filepath, std::string category = "default");

};

#endif