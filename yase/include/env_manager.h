#ifndef ENV_MANAGER_H
#define ENV_MANAGER_H


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


using namespace ENGINE;


	
	class EnvManager
	{
		inline static std::string folders[6] { "texture", "skybox", "model","shader", "sound", "scene"};
		inline static std::string env_file_name = "env.yase";


		fs::path			root_folder;
		std::string				name;

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

		std::string getName() {
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


		EnvManager();

		bool createNew(fs::path directory,std::string name);

		bool save();


		bool load(fs::path filepath);

	};

#endif