#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <iostream>
#include <filesystem>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "logger.h"
#include "texture.pb.h"
#include "textures.h"

#include "base_manager.h"

namespace fs = std::filesystem;
using namespace std;


// TextureManager s'occupe de gerrer nos textures et de loader
// ceux qu'on veut utiliser quand le temps est venu
class TextureManager : public AssetManager {
	int									index_texture = 0;
	ENGINE::MyTexture					texture_loader; // Classe pour charger les textures
	std::vector<YASE::DEF::Texture>		textures; // Liste de nos textures dans notre repertoire 
	std::vector<std::string>			categories;
	std::map<int, uint>					loaded_textures; // Liste des textures loader dans openGL, Key c'est leur ID

	bool								is_loaded = false;

public:
	virtual void saveManager(ostream* writer) {
		YASE::DEF::TextureManager tm;
		tm.set_index(index_texture);
		for (auto i : textures)
		{
			auto* t = tm.add_textures();
			*t = i;
		}
		if (!tm.SerializeToOstream(writer)){

		}
	}

	virtual void loadManager(ifstream* reader) {
		YASE::DEF::TextureManager env;
		if (!env.ParseFromIstream(reader)) {
		}
		this->index_texture = env.index();
		for (int i = 0; i < env.textures_size(); i++)
		{
			if (env.textures(i).name().empty())
				continue;
			this->textures.emplace_back(env.textures(i));
		}
	}

	TextureManager() :
		texture_loader(GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB) {
		file_name = "tex.yase";
	}

	int getCurrentIndex()
	{
		return index_texture;
	}

	bool isLoaded() const
	{
		return is_loaded;
	}

	vector<YASE::DEF::Texture>&	getTextures()
	{
		return textures;
	}

	const YASE::DEF::Texture& getTexture(int index)
	{
		for(const auto& t : textures)
		{
			if (t.id() == index)
				return t;
		}
	}

	map<int,uint>&	getLoadedTextures()
	{
		return loaded_textures;
	}
	
	vector<string> getCategories() const {
		return categories;
	}

	int getTexturesCount() const {
		return textures.size();
	}

	int getTexturesLoadedCount() const {
		return loaded_textures.size();
	}

	bool isTexLoaded(int index)
	{
		return loaded_textures.count(index) > 0;
	}

	uint getTexGLID(int index)
	{
		auto v = loaded_textures.find(index);
		if (v == loaded_textures.end())
			return ERROR_TEXTURE;
		return v->second;
	}

	vector<uint> loadTexture(vector<int> index)
	{
		vector<uint> v;
		for(const auto& i : index)
		{
			uint tid = loadTexture(i);
			if (tid == ERROR_TEXTURE)
				continue;
			v.emplace_back(tid);
		}
		return v;
	}

	uint loadTexture(int index)
	{
		if(isTexLoaded(index))
		{
			return getTexGLID(index);
		}
		auto v = getTexture(index);
		fs::path p = root_folder / v.category();
		p = p / v.name();
		uint tid = texture_loader.GetTexture(p.string());
		if (tid == ERROR_TEXTURE)
			return tid;
		loaded_textures[index] = tid;
		return tid;
	}




	// AddNewTexture ajoute une nouvelle texture dans
	// la liste et cree son information
	bool AddNewTexture(fs::path filepath, string category = "default") {
		if (!fs::exists(filepath))
			return false;
		fs::path dest = root_folder / category;
		if (!fs::exists(dest)) {
			YASE_LOG_INFO(("Creation du repertoire pour la categorie " + category).c_str());
			if (!fs::create_directory(dest)) {
				YASE_LOG_ERROR(("Echec de la creation repertoire" + dest.string()).c_str());
				return false;
			}
			categories.emplace_back(category);
		}
		dest = dest / filepath.filename();
		if(fs::exists(dest))
		{
			return true; // retourne true si l'image existe deja dans le fichier de destination
		}
		if (!fs::exists(dest) && !fs::copy_file(filepath, dest)) {
			YASE_LOG_ERROR(("Echec de la copie du ficher" + filepath.string()).c_str());
			return false;
		}
		YASE::DEF::Texture t;
		t.set_name(filepath.filename().string());
		uint v = texture_loader.GetTexture(dest.string());
		if (v == ERROR_TEXTURE) {
			return false;
		}
		ENGINE::TextureFileInfo fi = texture_loader.getLastTextureInfo();
		t.set_category(category);
		t.set_channels(fi.channel);
		t.set_height(fi.height);
		t.set_width(fi.width);
		t.set_id(index_texture);
		index_texture++;
		textures.emplace_back(t);
		loaded_textures[t.id()] = v;
		return true;
	}
};

#endif