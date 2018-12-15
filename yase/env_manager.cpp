#include "env_manager.h"

EnvManager::EnvManager()
		{

		}


		bool EnvManager::createNew(fs::path directory,std::string name) {
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

		bool EnvManager::save()
		{
			fs::path p = root_folder / env_file_name;
			Environment env;
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

		bool EnvManager::load(fs::path filepath) {
			yas::file_istream ifs(filepath.string().c_str());
			Environment env;
			
			yas::load<flags_yas_bf>(ifs, env, skybox_manager,texture_manager,scene_manager,model_manager);
			this->name = env.name;
			this->root_folder = filepath.parent_path();
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