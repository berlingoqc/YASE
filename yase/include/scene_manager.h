#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H
#include "texture_manager.h"
#include "skybox_manager.h"
#include "model_manager.h"
#include "camera.h"
#include <glm/vec3.hpp>
#include "shaders.h"
#include "model.h"

class SkyBoxGenerator
	{
		uint vao = 0;

		uint texture = 0;
		
		inline static const float skyboxVertices[] = {
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

	public:


		void Allocate();
		void setTexture(uint t)
		{
			texture = t;
		}
		void Draw(uint shader);

	};


struct KeyFrame
{
	float			timestamp;
	MatriceModel	transformation;
};

struct AnimationModel
{
	std::string				name;
	std::string				model_key;

	std::vector<KeyFrame>	frames;
	float				total_length;
};

struct StaticModel
{
	// Champs pour la serialisation 
	std::string model_key; 
	MatriceModel transformation;

	// Valeur Obtenue
	glm::mat4	mat_model = glm::mat4(1.0f);

	YaseModel*	model = nullptr;

	void updateModel()
	{
		transformation.updateModel(mat_model);
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


	template<typename Ar>
	void serialize(Ar& ar) {
		ar & YAS_OBJECT(nullptr, model_key, transformation);
	}
};

class BaseScene
{
public:
	std::string							name;

	std::vector<std::string>					needed_model; // Les cles des models requis

	// Get le pointeur vers les manager pour loader nos assets et nos shits
	TextureManager*					tex_manager = nullptr;
	SkyBoxManager*					skybox_manager = nullptr;
	ModelManager*					model_manager = nullptr;

	// Generator qui s'occupe d'afficher notre skybox selectionner
	SkyBoxGenerator					sky_box_generator;
	
	// Nom de la skybox active
	std::string							active_skybox;

	FPSCamera						work_camera;
	std::map<std::string,FPSCamera>			scene_cameras;

	ENGINE::Shrapper				shader_texture;
	ENGINE::Shrapper				shader_skybox;


	int								w_height, w_width;
	float							near_plane = 0.1f;
	float							rear_plane = 800.0f;

	bool							is_loaded = false;

	std::map<std::string, StaticModel*>		env_model_map; // Contient les models static qui compose l'environnement de la scene

	std::vector<StaticModel>				env_model;

public:

	template<typename Ar>
	void serialize(Ar& ar) {
		ar & YAS_OBJECT(nullptr, name, needed_model, active_skybox, work_camera, scene_cameras, near_plane, rear_plane, env_model);
	}

	void generateMap() {
		for (auto& i : env_model) {
			env_model_map[i.model_key] = &i;
			i.loadStaticModel(model_manager);
		}
	}


	BaseScene();
	
	BaseScene(TextureManager* tm, SkyBoxManager* sbm, ModelManager* mm, int height, int width);

	void setSceneSize(int w, int h) {
		w_height = h;
		w_width = w;
	}


	// Retourne le pointeur de la camera de l'environnement
	FPSCamera* getWorkingCamera()
	{
		return &work_camera;
	}

	// Retourne le pointeur d'une des camera de la scene , si elle existe
	FPSCamera* getSceneCamera(std::string name)
	{
		
		return nullptr;
	}

	void setActiveSkybox(std::string name);
	
	void removeEnvironmentModel(std::string name);

	void addEnvironmentModel(std::string name, std::string model_key);

	void loadGL();

	void Draw();
};



class SceneManager : public AssetManager {

	std::map<std::string,BaseScene>	scene;


	std::string					active_scene_name = "";
	BaseScene*				active_scene = nullptr;

public:
	// Get le pointeur vers les manager pour loader nos assets et nos shits
	TextureManager*					tex_manager = nullptr;
	SkyBoxManager*					skybox_manager = nullptr;
	ModelManager*					model_manager = nullptr;

	template<typename Ar>
	void serialize(Ar& ar) {
		ar & YAS_OBJECT(nullptr, scene, active_scene_name);
	}

	std::vector<const char*> getKeys()
	{
		std::vector<const char*> v;
		for (const auto& t : scene)
			v.emplace_back(t.first.c_str());
		return v;
	}

	virtual void saveManager(std::ostream* writer) override
	{
		
		
	}

	virtual void loadManager(std::ifstream* reader) override
	{
		
		
	}

	SceneManager();

	bool addNewScene(std::string name,int width, int height);


	BaseScene* loadScene(std::string name);


	BaseScene* getActiveScene()
	{
		return active_scene;
	}

};

#endif