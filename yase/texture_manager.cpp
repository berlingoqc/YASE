#include "texture_manager.h"

TextureManager::TextureManager() :
		texture_loader(GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB) {
		file_name = "tex.yase";
	}

	uint TextureManager::getTexGLID(std::string name)
	{
		YaseTextureInfo* t = getLoadedTextures(name);
		if (t == nullptr)
			return ERROR_TEXTURE;
		getTexGLID(t);
		return t->gl_id;
	}

	void TextureManager::getTexGLID(YaseTextureInfo* t) {
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

	std::vector<uint> TextureManager::loadTexture(std::vector<std::string> keys)
	{
		std::vector<uint> v;
		for(const auto& i : keys)
		{
			uint tid = getTexGLID(i);
			if (tid == ERROR_TEXTURE)
				continue;
			v.emplace_back(tid);
		}
		return v;
	}

	YaseTextureInfo* TextureManager::loadTexture(std::string key) {


        return nullptr;

	}
	void TextureManager::AddNewLoadedTexture(std::string name, std::string tex_name, TexWrappingOptions w, TexFilterOptions f) {
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


	bool TextureManager::AddNewTexture(std::string filename,char* data, int size,std::string category) {
		printf("Ajout nouvelle texture deja loader %d \n", size);
		Texture t;
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
	bool TextureManager::AddNewTexture(fs::path filepath, std::string category) {
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
		Texture t;
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