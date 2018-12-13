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

using namespace std;


struct YaseTextureInfo {
	YASE::DEF::Texture			texture; // Contient l'informations de la texture
	YASE::DEF::TextureOption	option; // Contient les informations de chargement de la texture
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
	ENGINE::MyTexture					texture_loader; // Classe pour charger les textures
	std::vector<YASE::DEF::Texture>		textures; // Liste de nos textures dans notre repertoire
	std::vector<std::string>			categories; // Liste des catégories de texture ??
	map<string, YaseTextureInfo>		loaded_textures; // Contient les informations sur des textures avec les info de loading utiliser par un context donner

	int nbr_loaded_gl = 0;

	virtual void saveManager(ostream* writer) {

	}

	virtual void loadManager(ifstream* reader) {

	}

	template<typename Ar>
	void serialize(Ar& ar)
	{
		ar & YAS_OBJECT(nullptr,textures,categories,loaded_textures);
	}

	TextureManager() :
		texture_loader(GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB) {
		file_name = "tex.yase";
	}

	// Retourne la liste des fichiers de textures existants
	vector<YASE::DEF::Texture>&	getTextures()
	{
		return textures;
	}

	// Retourne la map des textures qui ont besoin d'etre loader ( avec leur settings )
	const map<string, YaseTextureInfo>& getLoadedtextures() {
		return loaded_textures;
	}


	// Retourne les informations d'une texture loader ( dependence d'un model )
	YaseTextureInfo* getLoadedTextures(string name) {
		if (const auto& i = loaded_textures.find(name); i != loaded_textures.end()) {
			return &i->second;
		}
		return nullptr;
	}


	// Retourne la listes des categories existantes
	vector<string> getCategories() const {
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

	// Retourne le nombre de texture chargé dans opengl ( qui ont un handler)
	int getGltextureCount() const {
		return nbr_loaded_gl;
	}

	uint getTexGLID(string name)
	{
		YaseTextureInfo* t = getLoadedTextures(name);
		if (t == nullptr)
			return ERROR_TEXTURE;
		getTexGLID(t);
		return t->gl_id;
	}

	void getTexGLID(YaseTextureInfo* t) {
		if(t->texture.size_data != 0) {
			t->gl_id = texture_loader.GetTexture(t->texture.data);
		}
		else {
			fs::path p = root_folder;
			p = p / t->texture.name;
			// Configure selon les parametre le texture loader
			t->gl_id = texture_loader.GetTexture(p.string());
		}
		if (t->gl_id != ERROR_TEXTURE)
			nbr_loaded_gl++;
	}

	vector<uint> loadTexture(vector<string> keys)
	{
		vector<uint> v;
		for(const auto& i : keys)
		{
			uint tid = getTexGLID(i);
			if (tid == ERROR_TEXTURE)
				continue;
			v.emplace_back(tid);
		}
		return v;
	}

	YaseTextureInfo* loadTexture(string key) {

	}
	void AddNewLoadedTexture(string name, string tex_name, YASE::DEF::TexWrappingOptions w, YASE::DEF::TexFilterOptions f) {
		// valide que la cle n'existe pas deja
		YaseTextureInfo yti;
		for(const auto& i : textures)
		{
			if(i.name == tex_name)
			{
				yti.texture = i;
				break;
			}
		}
		if(yti.texture.name.empty())
		{
			// throw not found
			return;
		}
		yti.option.filter = f;
		yti.option.wrapping = w;
		loaded_textures[name] = yti;
	}


	bool AddNewTexture(string filename,char* data, int size,string category = "default") {
		printf("Ajout nouvelle texture deja loader %d \n", size);
		YASE::DEF::Texture t;
		t.name = filename;
		t.size_data = size;

		t.data = texture_loader.GetContent((unsigned char*)data,size);
		printf("Get Content is done\n");
		uint v = texture_loader.GetTexture(t.data);
		if (v == ERROR_TEXTURE) {
			// Devrait tu supprimer le fichier ? oui
			printf("Echer generer %s\n", filename.c_str());
			return false;
		}
		printf("Prout\n");
		ENGINE::TextureFileInfo fi = texture_loader.getLastTextureInfo();
		t.category = category;
		t.channels = fi.channel;
		t.height = fi.height;
		t.width = fi.width;
		textures.emplace_back(t);
		printf("End add texture\n");
		glDeleteTextures(1, &v);
		return true;
	}
	// AddNewTexture ajoute une nouvelle texture dans
	// la liste et cree son information
	bool AddNewTexture(fs::path filepath, string category = "default") {
		if (!fs::exists(filepath)) {
			return false;
		}
		fs::path dest = root_folder;
		if (!fs::exists(dest)) {
			YASE_LOG_INFO(("Creation du repertoire pour la categorie " + category).c_str());
			return false;
		}
		dest = dest / filepath.filename();
		if(fs::exists(dest))
		{
			return true;  // retourne true si l'image existe deja dans le fichier de destination
		}
		if (!fs::exists(dest) && !fs::copy_file(filepath, dest)) {
			YASE_LOG_ERROR(("Echec de la copie du ficher" + filepath.string()).c_str());
			return false;
		}
		YASE::DEF::Texture t;
		t.name = filepath.filename().string();
		uint v = texture_loader.GetTexture(dest.string());
		if (v == ERROR_TEXTURE) {
			// Devrait tu supprimer le fichier ? oui
			return false;
		}
		ENGINE::TextureFileInfo fi = texture_loader.getLastTextureInfo();
		t.category = category;
		t.channels = fi.channel;
		t.height=fi.height;
		t.width = fi.width;
		textures.emplace_back(t);

		glDeleteTextures(1,&v);
		return true;
	}
};

#endif