#ifndef SKYBOX_MANAGER_H
#define SKYBOX_MANAGER_H

#include "base_manager.h"
#include "textures.h"
#include "logger.h"

typedef std::map<std::string, std::tuple<YASE::DEF::SkyBox, uint>> MapSkyBox;

class SkyBoxManager : public AssetManager
{
	inline const static std::string									skybox_files[6]{ "right", "left", "top", "bot", "front", "back" };

	ENGINE::MyTexture												texture_loader;
	MapSkyBox														boxes;
	int																loaded_skybox_index = 0;
	
public:


	template<typename Ar>
	void serialize(Ar& ar) {
		for (auto& b : boxes) {
			std::get<1>(b.second) = ERROR_TEXTURE;
		}
		ar & YAS_OBJECT(nullptr, boxes);
	}


	vector<const char*> getKeys()
	{
		vector<const char*> v;
		for (const auto& t : boxes)
			v.emplace_back(t.first.c_str());
		return v;
	}

	virtual void saveManager(ostream* writer) {

	}

	virtual void loadManager(ifstream* reader) {

	}

	SkyBoxManager() : texture_loader(GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB)
	{
		file_name = "skybox.yase";
	}

	int getNumberSkyBox() const
	{
		return boxes.size();
	}

	int getNumberLoadedSkyBox() const
	{
		return loaded_skybox_index;
	}

	bool isLoaded(string key)
	{
		const auto& v = boxes.find(key);
		if (v == boxes.end())
			return false;
		return (std::get<1>(v->second) != ERROR_TEXTURE);
	}

	const MapSkyBox& getMapSkyBox() const
	{
		return boxes;
	}


	bool AddSkyBox(const std::string& name, const fs::path& directory)
	{
		// Regarde que le nom est pas deja dans le map
		if (boxes.count(name) > 0) {
			YASE_LOG_ERROR(("Impossible d'ajouter skybox cle deja presente " + name + "\n").c_str());
			return false;
		}
		if (!fs::exists(directory)) {
			YASE_LOG_ERROR(("Le repertoire n'existe pas " + directory.string() + "\n").c_str());
			return false;
		}

		// Regarde que le repertoire existe et qu'il contient les 5 fichiers necessaire
		// dans la meme extension
		std::string ext = "";
		int		nbr_found = 0;
		for(const auto& f : fs::directory_iterator(directory))
		{
			fs::path fp = fs::path(f);
			if (fs::is_directory(fp)) { // si on trouve un repertoire en dedans  c'est pas bon
				YASE_LOG_ERROR(("Impossible cree skybox il y a un repertoire dans le dossier source " + fp.string() + "\n").c_str());
				return false;
			}
			if (ext.empty())
				ext = fp.extension().string();
			for(const auto& file : skybox_files)
			{
				if(fp.filename() == file + ext)
				{
					nbr_found++;
					break;
				}
			}
		}
		if (nbr_found != 6) {
			YASE_LOG_ERROR("Impossible de trouver toute les textures necessaire dans le repertoire\n");
			return false;
		}

		// Copie le repertoire et load la skybox
		fs::path dest = root_folder / name;
		if (fs::exists(dest)) {
			YASE_LOG_ERROR(("Le dossier de destination existe deja "+ dest.string()+"\n").c_str());
			return false;
		}
		fs::copy(directory, dest);
		YASE::DEF::SkyBox b;
		b.name = name;
		b.ext = ext;
		boxes[name] = std::make_tuple(b, ERROR_TEXTURE);
		return true;
	}



	uint LoadSkyBox(std::string name)
	{
		const auto& b = boxes.find(name);
		if (b == boxes.end())
			return ERROR_TEXTURE;
		if (std::get<1>(b->second) != ERROR_TEXTURE)
			return std::get<1>(b->second);
		vector<string> faces;
		const auto sb = std::get<0>(b->second);
		for(int i = 0;i<6;i++)
		{
			fs::path p = root_folder / sb.name;
			p = p / skybox_files[i];
			faces.emplace_back(p.string().append(sb.ext));
		}
		uint tid = texture_loader.GetTextureSky(faces);
		loaded_skybox_index++;
		std::get<1>(boxes[name]) = tid;
		return tid;
	}
};

#endif