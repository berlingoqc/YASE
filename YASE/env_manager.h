#ifndef ENV_MANAGER_H
#define ENV_MANAGER_H

#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include "textures.h"
#include "logger.h"

#include "texture_manager.h"
#include "shader_manager.h"
#include "skybox_manager.h"

#include "base.pb.h"
#include "env.pb.h"

using namespace std;
using namespace ENGINE;

namespace fs = std::filesystem;


	class EnvManager
	{
		inline static string folders[5] { "texture", "skybox", "mesh", "asset", "sound" };
		inline static string env_file_name = "env.yase";

		fs::path			root_folder;
		string				name;

		TextureManager		texture_manager;
		SkyBoxManager		skybox_manager;
		ShadersManager		shader_manager;

	public:
		fs::path getRootFolder() {
			return root_folder;
		}

		string getName() {
			return name;
		}

		TextureManager*	getTextureManager() {
			return &texture_manager;
		}

		SkyBoxManager* getSkyBoxManager()
		{
			return &skybox_manager;
		}

		EnvManager()
		{

		}

		bool createNew(fs::path directory,string name) {
			if (name.empty() || !fs::exists(directory))
				return false;
			for (const auto& f : folders) {
				fs::path nf = directory / f;
				if(fs::exists(nf))
				{
					return false;
				}
				if (!fs::create_directory(nf))
					return false;
			}
			root_folder = directory;
			this->name = name;

			texture_manager.setRootFolder(root_folder / folders[0]);
			skybox_manager.setRootFolder(root_folder / folders[1]);
			if (!texture_manager.Save())
				return false;
			if (!skybox_manager.Save())
				return false;
			return save();
		}

		bool save()
		{
			YASE::DEF::Environment env;
			env.set_name(this->name);
			env.set_root(root_folder.string());

			// Get les variable de sauvegarde des autres aussi

			fs::path fn = root_folder / env_file_name;
			ofstream of(fn.string(), std::ios::binary);
			if (!env.SerializeToOstream(&of))
				return false;
			if (!skybox_manager.Save())
				return false;
			return texture_manager.Save();
		}

		bool load(fs::path filepath) {
			ifstream inf;
			inf.open(filepath, ios::binary | std::ios::in);
			if (!inf.is_open())
				return false;
			YASE::DEF::Environment env;
			if (!env.ParseFromIstream(&inf))
				return false;
			inf.close();
			this->name = env.name();
			this->root_folder = filepath.parent_path();

			fs::path tex_root = this->root_folder / folders[0];
			texture_manager.setRootFolder(tex_root);
			tex_root = this->root_folder / folders[1];
			skybox_manager.setRootFolder(tex_root);
			if (!skybox_manager.Load())
				return false;
			return texture_manager.Load();
		}
	};

#endif