#ifndef SCENE_H
#define SCENE_H
#include "texture_manager.h"
#include "skybox_manager.h"
#include "skybox_manager.h"
#include "camera.h"
#include <glm/vec3.hpp>
#include "shaders.h"


class SkyBoxGenerator
	{
		uint vao = 0;

		uint texture = 0;


	public:


		void Allocate()
		{
			float skyboxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};
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


		void setTexture(uint t)
		{
			texture = t;
		}

		void Draw(uint shader)
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

	};


	class BaseScene
	{

		TextureManager*		tex_manager;
		SkyBoxManager*		skybox_manager;

		SkyBoxGenerator		sky_box_generator;


		string				active_skybox;

		FPSCamera			work_camera;

		ENGINE::Shrapper	shader_texture;
		ENGINE::Shrapper	shader_skybox;

		int					w_height, w_width;

		bool				is_loaded = false;
		YaseModel			model;

	public:
		BaseScene(TextureManager* tm, SkyBoxManager* sbm) 
		{
			tex_manager = tm;
			skybox_manager = sbm;
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
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			sky_box_generator.Allocate();

			YASE::DEF::Model m = getDefaultModel();
			model.LoadFromDefModel(m);

		}

		void LoadModel()
		{
			model.LoadModel(tex_manager);
			is_loaded = true;
		}

		void setSceneSize(int w, int h)
		{
			w_height = h;
			w_width = w;
		}


		FPSCamera* getWorkingCamera()
		{
			return &work_camera;
		}


		void setActiveSkybox(string name)
		{
			uint u = skybox_manager->LoadSkyBox(name);
			if (u == ERROR_TEXTURE)
				return;
			active_skybox = name;
			sky_box_generator.setTexture(u);
		}


		void Draw()
		{
			glm::mat4	view;
			glm::mat4	projection;
			glm::mat4	modele;

			projection = glm::perspective(glm::radians(work_camera.getFOV()), (w_width / w_height) * 1.0f, 0.1f, 800.0f);

			glDepthMask(GL_FALSE);
			shader_skybox.Use();
			view = work_camera.getView();
			shader_skybox.setMat4("gProjection", projection);
			shader_skybox.setMat4("gVue", view);

			sky_box_generator.Draw(shader_skybox.getID());

			if (is_loaded) {
				shader_texture.Use();
				view = work_camera.getLookAt();
				shader_texture.setMat4("gProjection", projection);
				shader_texture.setMat4("gVue", view);
				modele = glm::mat4(1.0);
				shader_texture.setMat4("gModele", modele);
				model.Draw(shader_texture.getID());
			}
				
		}


		void Save()
		{
			
		}

		void Load()
		{
			
		}
		
	};

#endif