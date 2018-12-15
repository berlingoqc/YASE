#include "skybox_manager.h"

SkyBoxManager::SkyBoxManager() : texture_loader(GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB)
{
	file_name = "skybox.yase";
}


	bool SkyBoxManager::isLoaded(std::string key)
	{
		const auto& v = boxes.find(key);
		if (v == boxes.end())
			return false;
		return (std::get<1>(v->second) != ERROR_TEXTURE);
	}


	bool SkyBoxManager::AddSkyBox(const std::string& name, std::vector<char*> faces,std::vector<int> sizes) {
		SkyBox b;
		b.name = name;
		b.ext = ".png";
		uint id = texture_loader.GetTextureSky(faces, sizes);
		if (id == ERROR_TEXTURE) {
			printf("Echer du chargement de %s\n", name.c_str());
			return false;
		}
		boxes[name] = std::make_tuple(b, id);
		return true;
	}


	bool SkyBoxManager::AddSkyBox(const std::string& name, const fs::path& directory)
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
		SkyBox b;
		b.name = name;
		b.ext = ext;
		boxes[name] = std::make_tuple(b, ERROR_TEXTURE);
		return true;
	}



	uint SkyBoxManager::LoadSkyBox(std::string name)
	{
		const auto& b = boxes.find(name);
		if (b == boxes.end())
			return ERROR_TEXTURE;
		if (std::get<1>(b->second) != ERROR_TEXTURE)
			return std::get<1>(b->second);
		std::vector<std::string> faces;
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