#include "scene_manager.h"


		void SkyBoxGenerator::Allocate()
		{

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			GLuint vertexbuffer;
			glGenBuffers(1, &vertexbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);

		}

		void SkyBoxGenerator::Draw(uint shader)
		{
			if (texture != 0) {
				glActiveTexture(GL_TEXTURE0);
				glUniform1i(glGetUniformLocation(shader, "Skybox"), 0);
				glBindVertexArray(vao);
				glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glDepthMask(GL_TRUE);
			}
		}


	BaseScene::BaseScene()
	{
		ENGINE::MyShader shader;
		if (!shader.OpenMyShader("default_shaders/vert_shader.glsl", "default_shaders/frag_shader.glsl"))
		{
			std::cout << "Echer lors de l'ouverture des shaders par default" << std::endl;
			return;
		}
		this->shader_texture = shader.GetShaderID();
		if (!shader.OpenMyShader("default_shaders/vert_skybox.glsl", "default_shaders/frag_skybox.glsl"))
		{
			std::cout << "Echer lors de l'ouverture des shaders de la skybox" << std::endl;
			return;
		}
		this->shader_skybox = shader.GetShaderID();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		sky_box_generator.Allocate();
	}
	BaseScene::BaseScene(TextureManager* tm, SkyBoxManager* sbm, ModelManager* mm, int height, int width)  : BaseScene()
	{
		tex_manager = tm;
		skybox_manager = sbm;
		model_manager = mm;
	}

	void BaseScene::setActiveSkybox(std::string name)
	{
		if(name.empty())
		{
			active_skybox = name;
			return;
		}
		uint u = skybox_manager->LoadSkyBox(name);
		if (u == ERROR_TEXTURE)
			return;
		active_skybox = name;
		sky_box_generator.setTexture(u);
	}

	void BaseScene::removeEnvironmentModel(std::string name)
	{
		if (const auto& p = env_model_map.find(name); p == env_model_map.end() || name.empty())
			return;
		if(env_model_map[name] !=nullptr)
		{
			delete env_model_map[name];
			env_model_map.erase(name);
		}
	}

	void BaseScene::addEnvironmentModel(std::string name, std::string model_key)
	{
		if(name.empty())
		{
			YASE_LOG_ERROR("Aucun nom pour la cle du model de l'environment\n");
			return;
		}
		if (const auto& p = env_model_map.find(name); p != env_model_map.end() || name.empty())
			return;
		StaticModel smodel;
		env_model.emplace_back(smodel);
		env_model_map[name] = &env_model[env_model.size() - 1];
		env_model_map[name]->model_key = model_key;
		env_model_map[name]->loadStaticModel(model_manager);
	}



	void BaseScene::loadGL()
	{
		setActiveSkybox(active_skybox);
		for(const auto& i : env_model_map)
		{
			i.second->loadStaticModel(model_manager);
		}
		is_loaded = true;
	}

	void BaseScene::Draw()
	{
		glm::mat4	view;
		glm::mat4	projection;
		glm::mat4	modele;

		projection = glm::perspective(glm::radians(work_camera.getFOV()), (w_width / w_height) * 1.0f, near_plane, rear_plane);

		// DRAW LA SKYBOX SI N'A UNE
		if (!active_skybox.empty()) {
			glDepthMask(GL_FALSE);
			shader_skybox.Use();
			view = work_camera.getView();
			shader_skybox.setMat4("gProjection", projection);
			shader_skybox.setMat4("gVue", view);
			sky_box_generator.Draw(shader_skybox.getID());
		}

		// DRAW LES OBJETCS STATIQUE DE L'ENVIRONMENT
		shader_texture.Use();
		view = work_camera.getLookAt();
		shader_texture.setMat4("gProjection", projection);
		shader_texture.setMat4("gVue", view);
		for(const auto& m : env_model_map)
		{
			if(m.second->model != nullptr)
			{
				shader_texture.setMat4("gModele", m.second->mat_model);
				m.second->model->Draw(shader_texture.getID());
			}
			
		}
	}



    
	SceneManager::SceneManager() {
		file_name = "scenes.yase";
	}

	bool SceneManager::addNewScene(std::string name,int width, int height) {
		if(const auto& v = scene.find(name); v == scene.end())
		{
			scene[name] = BaseScene(tex_manager, skybox_manager, model_manager, height, width);
			active_scene_name = name;
			active_scene = &scene[name];
			active_scene->name = name;
			return true;
		}
		return false;
	}

	BaseScene* SceneManager::loadScene(std::string name)
	{
		const auto& i = scene.find(name);
		if (i == scene.end())
			return nullptr;
		active_scene = &scene[name];
		active_scene->tex_manager = tex_manager;
		active_scene->model_manager = model_manager;
		active_scene->skybox_manager = skybox_manager;
		active_scene_name = name;
		active_scene->name = name;
		if(!active_scene->is_loaded)
		{
			i->second.loadGL();
		}

	
		return active_scene;
	}