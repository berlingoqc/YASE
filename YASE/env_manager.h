#ifndef ENV_MANAGER_H
#define ENV_MANAGER_H

#include "header.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "textures.h"
#include "logger.h"

#include "texture_manager.h"
#include "shader_manager.h"
#include "model_manager.h"
#include "skybox_manager.h"
#include "sound_manager.h"
#include "scene_manager.h"
#include "animation_manager.h"


using namespace std;
using namespace ENGINE;


	class EnvManager
	{
		inline static string folders[6] { "texture", "skybox", "model","shader", "sound", "scene"};
		inline static string env_file_name = "env.yase";

		fs::path			root_folder;
		string				name;

		TextureManager		texture_manager;
		SkyBoxManager		skybox_manager;
		ModelManager		model_manager;
		ShadersManager		shader_manager;
		SoundManager		sound_manager;

		SceneManager		scene_manager;



		void initFolderManager() {
			texture_manager.setRootFolder(root_folder / folders[0]);
			skybox_manager.setRootFolder(root_folder / folders[1]);
			model_manager.setRootFolder(root_folder / folders[2]);
			shader_manager.setRootFolder(root_folder / folders[3]);
			sound_manager.setRootFolder(root_folder / folders[4]);
			scene_manager.setRootFolder(root_folder / folders[5]);
		}

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

		ModelManager* getModelManager() {
			return &model_manager;
		}

		ShadersManager* getShaderManager() {
			return &shader_manager;
		}

		SoundManager* getSoundManager() {
			return &sound_manager;
		}

		SceneManager* getSceneManager() {
			return &scene_manager;
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
			this->root_folder = directory;
			this->name = name;

			initFolderManager();

			scene_manager.skybox_manager = &skybox_manager;
			scene_manager.model_manager = &model_manager;
			scene_manager.tex_manager = &texture_manager;

			model_manager.tex_manager = &texture_manager;

			texture_manager.Save();
			skybox_manager.Save();
			model_manager.Save();
			shader_manager.Save();
			sound_manager.Save();
			scene_manager.Save();

			return save();
		}

		bool save()
		{

			skybox_manager.Save();
			texture_manager.Save();
			shader_manager.Save();
			model_manager.Save();
			sound_manager.Save();
			scene_manager.Save();

			return true;
		}

		bool load(fs::path filepath) {
			ifstream inf;
			inf.open(filepath, ios::binary | std::ios::in);
			if (!inf.is_open())
				return false;
			YASE::DEF::Environment env;


			inf.close();

			this->name = env.name;
			this->root_folder = filepath.parent_path();

			initFolderManager();

			texture_manager.Load();
			skybox_manager.Load();
			shader_manager.Load();
			model_manager.Load();
			sound_manager.Load();
			scene_manager.Load();

			scene_manager.skybox_manager = &skybox_manager;
			scene_manager.model_manager = &model_manager;
			scene_manager.tex_manager = &texture_manager;

			model_manager.tex_manager = &texture_manager;

			return true;
		}
	};

#endif