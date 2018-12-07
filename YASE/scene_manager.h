#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H
#include "texture_manager.h"
#include "skybox_manager.h"
#include "model_manager.h"
#include "camera.h"
#include <glm/vec3.hpp>
#include "shaders.h"
#include "scene.pb.h"


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

struct MatriceModel
{
	glm::vec3	translation = glm::vec3(1,1,1);
	glm::vec3	scale = glm::vec3(1,1,1);
	glm::vec3	rotation = glm::vec3(0,0,0);
	float		angle_rotation = 0.0f;


	void save(YASE::DEF::MatriceModel* mm)
	{
		auto* p = mm->mutable_rotation();
		p->set_x(rotation.x);
		p->set_y(rotation.y);
		p->set_z(rotation.z);
		auto* t = mm->mutable_translation();
		t->set_x(translation.x);
		t->set_y(translation.y);
		t->set_z(translation.z);
		auto* s = mm->mutable_scale();
		s->set_x(scale.x);
		s->set_y(scale.y);
		s->set_z(scale.z);
		mm->set_angle_rotation(angle_rotation);
		
		
	}

	void load(const YASE::DEF::MatriceModel& mm)
	{
		auto& p = mm.translation();
		translation.x = p.x();
		translation.y = p.y();
		translation.z = p.z();
		auto& s = mm.scale();
		scale.x = s.x();
		scale .y = s.y();
		scale.z = s.z();
		auto& r = mm.rotation();
		rotation.x = r.x();
		rotation.y = r.y();
		rotation.z = r.z();
		angle_rotation = mm.angle_rotation();
	}
};

struct KeyFrame
{
	float			timestamp;
	MatriceModel	transformation;
};

struct AnimationModel
{
	string				name;
	string				model_key;

	vector<KeyFrame>	frames;
	float				total_length;
};

struct StaticModel
{
	// Champs pour la serialisation 
	string model_key; 
	MatriceModel transformation;

	// Valeur Obtenue
	glm::mat4	mat_model = glm::mat4(1.0f);

	YaseModel*	model = nullptr;

	void save(YASE::DEF::InitialModel* im)
	{
		im->set_model_key(model_key);
		auto* mt = im->mutable_transformation();
		transformation.save(mt);
	}

	void load(const YASE::DEF::InitialModel& im)
	{
		model_key = im.model_key();
		transformation.load(im.transformation());
	}

	void updateModel()
	{
		// Calcul ca matrice model
		mat_model = glm::mat4(1.0f);
		mat_model = glm::translate(mat_model, transformation.translation); // translate it down so it's at the center of the scene
		if(transformation.angle_rotation != 0)
			mat_model = glm::rotate(mat_model, glm::radians(transformation.angle_rotation), transformation.rotation);
		mat_model = glm::scale(mat_model, transformation.scale);	// it's a bit too big for our scene, so scale it down
	}

	void loadStaticModel(ModelManager* mm)
	{
		// Va cherche le pointeur de notre model
		model = mm->getLoadedModel(model_key);
		if(model == nullptr)
		{
			YASE_LOG_ERROR(("Impossible de charger le model " + model_key).c_str());
		}
		updateModel();
	}
};

class BaseScene
{
public:
	string							name;

	vector<string>					needed_model; // Les cles des models requis

	// Get le pointeur vers les manager pour loader nos assets et nos shits
	TextureManager*					tex_manager = nullptr;
	SkyBoxManager*					skybox_manager = nullptr;
	ModelManager*					model_manager = nullptr;

	// Generator qui s'occupe d'afficher notre skybox selectionner
	SkyBoxGenerator					sky_box_generator;
	
	// Nom de la skybox active
	string							active_skybox;

	FPSCamera						work_camera;
	map<string,FPSCamera>			scene_cameas;

	ENGINE::Shrapper				shader_texture;
	ENGINE::Shrapper				shader_skybox;


	int								w_height, w_width;
	float							near_plane = 0.1f;
	float							rear_plane = 800.0f;

	bool							is_loaded = false;

	map<string, StaticModel*>		env_model_map; // Contient les models static qui compose l'environnement de la scene

public:
	BaseScene()
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
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		sky_box_generator.Allocate();
	}
	BaseScene(TextureManager* tm, SkyBoxManager* sbm, ModelManager* mm, int height, int width)  : BaseScene()
	{
		tex_manager = tm;
		skybox_manager = sbm;
		model_manager = mm;
	}

	void setSceneSize(int w, int h)
	{
		w_height = h;
		w_width = w;
	}


	// Retourne le pointeur de la camera de l'environnement
	FPSCamera* getWorkingCamera()
	{
		return &work_camera;
	}

	// Retourne le pointeur d'une des camera de la scene , si elle existe
	FPSCamera* getSceneCamera(string name)
	{
		
		return nullptr;
	}

	void setActiveSkybox(string name)
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

	void removeEnvironmentModel(string name)
	{
		if (const auto& p = env_model_map.find(name); p == env_model_map.end() || name.empty())
			return;
		if(env_model_map[name] !=nullptr)
		{
			delete env_model_map[name];
			env_model_map.erase(name);
		}
	}

	void addEnvironmentModel(string name, string model_key)
	{
		if(name.empty())
		{
			YASE_LOG_ERROR("Aucun nom pour la cle du model de l'environment\n");
			return;
		}
		if (const auto& p = env_model_map.find(name); p != env_model_map.end() || name.empty())
			return;
		StaticModel* smodel = new StaticModel();
		smodel->model_key = model_key;
		smodel->loadStaticModel(model_manager);
		env_model_map[name] = smodel;
	}

	void save(YASE::DEF::Scene* scene)
	{
		scene->set_name(name);
		scene->set_skybox_name(active_skybox);
		auto* work_cam = scene->mutable_work_camera();
		work_camera.save(work_cam);

		// Sauvegarde nos models statique de l'environment
		for(const auto& i : env_model_map)
		{
			YASE::DEF::InitialModel* im = scene->add_initial_models();
			im->set_name(i.first);
			i.second->save(im);
		}
		
	}

	void load(const YASE::DEF::Scene& scene)
	{
		name = scene.name();
		active_skybox = scene.skybox_name();
		const auto& wc = scene.work_camera();
		work_camera.load(wc);

		// Load nos models statique de l'environment
		for(int i = 0;i<scene.initial_models_size();i++)
		{
			const auto& im = scene.initial_models(i);
			StaticModel* sm = new StaticModel();
			sm->load(im);
			env_model_map[im.name()] = sm;
		}

	}

	void loadGL()
	{
		setActiveSkybox(active_skybox);
		for(const auto& i : env_model_map)
		{
			i.second->loadStaticModel(model_manager);
		}
		is_loaded = true;
	}

	void Draw()
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

};



class SceneManager : public AssetManager {

	map<string,BaseScene>	scene;


	string					active_scene_name = "";
	BaseScene*				active_scene = nullptr;

public:
	// Get le pointeur vers les manager pour loader nos assets et nos shits
	TextureManager*					tex_manager = nullptr;
	SkyBoxManager*					skybox_manager = nullptr;
	ModelManager*					model_manager = nullptr;

	vector<const char*> getKeys()
	{
		vector<const char*> v;
		for (const auto& t : scene)
			v.emplace_back(t.first.c_str());
		return v;
	}

	virtual void saveManager(ostream* writer) override
	{
		YASE::DEF::ListScene ls;
		for(auto& s : scene)
		{
			YASE::DEF::Scene* ds = ls.add_scenes();
			s.second.save(ds);
		}
		if(!ls.SerializeToOstream(writer))
		{
			
		}
		
	}

	virtual void loadManager(ifstream* reader) override
	{
		YASE::DEF::ListScene ls;
		if(!ls.ParseFromIstream(reader))
		{
			return;
		}
		for(int i = 0; i < ls.scenes_size();i++)
		{
			const auto& s = ls.scenes(i);
			scene[s.name()] = BaseScene(tex_manager, skybox_manager, model_manager, 1600, 900);
			active_scene_name = s.name();
			active_scene = &scene[s.name()];
			active_scene->name = s.name();
			active_scene->active_skybox = s.skybox_name();
			active_scene->work_camera.load(s.work_camera());
			active_scene->load(s);
		}
		
	}

	SceneManager() {
		file_name = "scenes.yase";
	}

	bool addNewScene(string name,int width, int height) {
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

	BaseScene* loadScene(string name)
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

	BaseScene* getActiveScene()
	{
		return active_scene;
	}

};

#endif