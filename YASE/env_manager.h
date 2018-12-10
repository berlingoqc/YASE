#ifndef ENV_MANAGER_H
#define ENV_MANAGER_H

#include "header.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "textures.h"
#include "logger.h"

#include "base_manager.h"
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

			return save();
		}

		bool save()
		{
			fs::path p = root_folder / env_file_name;
			YASE::DEF::Environment env;
			auto* scene = scene_manager.getActiveScene();
			if (scene != nullptr) {
				env.active_scene = scene->name;
			}
			env.name = name;
			env.root = root_folder.string();

			yas::file_ostream of(p.string().c_str(), yas::file_trunc);
			yas::save<flags_yas_bf>(of,env,skybox_manager,texture_manager,scene_manager,model_manager);
			of.flush();


			return true;
		}

		bool load(fs::path filepath) {
			yas::file_istream ifs(filepath.string().c_str());
			YASE::DEF::Environment env;
			yas::load<flags_yas_bf>(ifs, env, skybox_manager,texture_manager,scene_manager,model_manager);
			this->name = env.name;
			this->root_folder = env.root;
			if (!env.active_scene.empty()) {
				// reload la scene active
			}
			model_manager.generateMap();

			initFolderManager();

			scene_manager.skybox_manager = &skybox_manager;
			scene_manager.model_manager = &model_manager;
			scene_manager.tex_manager = &texture_manager;

			model_manager.tex_manager = &texture_manager;
			

			return true;
		}
	};

#endif