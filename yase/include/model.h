#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <yas/serialize.hpp>

	enum TexWrappingOptions { REPEATED, MIRRORED_REPEATED, CLAMP_EDGE, CLAMP_BORDER };

	enum TexFilterOptions { NEAREST, LINEAR };

	struct Texture
	{
		std::string		name; // nom du fichier
		std::string		category; // category de la texture

		unsigned char*		data;
		int			size_data;

		int			width;
		int			height;
		int			channels;

		Texture() {}
		Texture(std::string n,std::string cat,int w, int h, int c) : name(n), category(cat), width(w), height(h), channels(c) {}

		template<typename Ar>
		void serialize(Ar &ar) {
			ar & YAS_OBJECT(nullptr, name, category, width, height, channels);
		}
	};

	struct TextureOption
	{
		TexWrappingOptions	wrapping;
		TexFilterOptions	filter;


		template<typename Ar>
		void serialize(Ar &ar) {
			ar & YAS_OBJECT(nullptr, wrapping, filter);
		}
	};


	struct SkyBox
	{
		std::string		name;
		std::string		ext;

		int			total_size;

		SkyBox() : name(), ext(), total_size() {}
		SkyBox(std::string n,std::string e,int ts) : name(n), ext(e),total_size(ts) {}

		template<typename Ar>
		void serialize(Ar& ar) {
			ar & YAS_OBJECT(nullptr, name, ext, total_size);
		}
	};


	enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER, TESSELATION_SHADER };

	struct ShaderFile
	{
		std::string		file_name;
		std::string		version;
		ShaderType	type;
	};

	struct ShaderProgram
	{
		std::string		name;
		ShaderFile	vertex_shader;
		ShaderFile	fragment_shader;
		ShaderFile	geometry_shader;
		ShaderFile	tesselation_shader;
	};

	struct ShaderManager
	{
		std::vector<ShaderFile>		files;
		std::vector<ShaderProgram>	programs;

	};

	struct Vec3f
	{
		float x;
		float y;
		float z;

		Vec3f() : x(), y(), z() {}
		Vec3f(float x1, float y1, float z1) : x(x1), y(y1), z(z1) {
			
		}

		template<typename Ar>
		void serialize(Ar& ar) {
			ar & YAS_OBJECT(nullptr, x, y, z);
		}
	};

	struct Vec2f
	{
		float x;
		float y;

		template<typename Ar>
		void serialize(Ar& ar) {
			ar & YAS_OBJECT(nullptr, x, y);
		}
	};

	struct Vertex
	{

		Vec3f	Position;
		Vec3f	Normal;
		Vec2f	TexCoords;

		Vec3f	Tangent;
		Vec3f	Bitangent;

		template<typename Ar>
		void serialize(Ar& ar) {
			ar & YAS_OBJECT(nullptr, Position, Normal, TexCoords, Tangent, Bitangent);

		}

	};

	struct Mesh
	{

		std::vector<Vertex>			vertices;
		std::vector<unsigned int>	indices;
		std::vector<std::string>			texture_keys;

	};

	struct Model
	{
		std::string				name;
		std::vector<Mesh>	meshes;
	};

	struct CameraPosition
	{
		std::string	name;

		Vec3f	front;
		Vec3f	position;
		Vec3f	height;

		float	yaw;
		float	pitch;

		float	fov;


		Vec3f	speed;
	};


	struct MatriceModel
	{
	Vec3f	translation = Vec3f(1.0f,1.0f,1.0f);
	Vec3f	scale = Vec3f(1.0f,1.0f,1.0f);
	Vec3f	rotation = Vec3f(1.0f,1.0f,1.0f);
	float				angle_rotation = 0.0f;


	void updateModel(glm::mat4& mat_model,bool newmodel = true) {
		// Calcul ca matrice model
		if(newmodel)
			mat_model = glm::mat4(1.0f);
		const auto t = glm::vec3(translation.x, translation.y, translation.z);
		const auto s = glm::vec3(scale.x, scale.y, scale.z);
		const auto r = glm::vec3(rotation.x, rotation.y, rotation.z);
		mat_model = glm::translate(mat_model, t); // translate it down so it's at the center of the scene
		if (angle_rotation != 0)
			mat_model = glm::rotate(mat_model, glm::radians(angle_rotation), r);
		mat_model = glm::scale(mat_model, s);	// it's a bit too big for our scene, so scale it down
	}

	template<typename Ar>
	void serialize(Ar& ar) {
		ar & YAS_OBJECT(nullptr, translation, scale, rotation, angle_rotation);
	}
	};


	struct InitialModel
	{
		std::string	model_key;
		std::string  name;

		MatriceModel transformation;
	};


	struct Scene
	{
		std::string					name;

		std::string					skybox_name;

		std::vector<std::string>			needed_model;

		CameraPosition			work_camera;
		std::vector<CameraPosition>	scene_camera;
	};

	struct Environment
	{
		std::string		name;
		std::string		root;


		std::string		active_scene;

		std::vector<Scene> scenes;

		Environment() : name(), root(), active_scene(), scenes() {

		}
		Environment(std::string n, std::string r, std::string as, std::vector<Scene> s) : name(n), root(r), active_scene(as), scenes(s) {

		}

		template<typename Ar>
		void serialize(Ar& ar) {
			ar & YAS_OBJECT(nullptr, name, root, active_scene);
		}
	};


#endif