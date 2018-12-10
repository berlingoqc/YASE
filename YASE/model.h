#ifndef MODEL_H
#define MODEL_H

#include <yas/serialize.hpp>

namespace YASE::DEF
{
	enum TexWrappingOptions { REPEATED, MIRRORED_REPEATED, CLAMP_EDGE, CLAMP_BORDER };

	enum TexFilterOptions { NEAREST, LINEAR };

	struct Texture
	{
		string		name; // nom du fichier
		string		category; // category de la texture

		int			width;
		int			height;
		int			channels;

		Texture() : name(), category(), width(), height(), channels() {}
		Texture(string n,string cat,int w, int h, int c) : name(n), category(cat), width(w), height(h), channels(c) {}

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
		string		name;
		string		ext;

		int			total_size;

		SkyBox() : name(), ext(), total_size() {}
		SkyBox(string n,string e,int ts) : name(n), ext(e),total_size(ts) {}

		template<typename Ar>
		void serialize(Ar& ar) {
			ar & YAS_OBJECT(nullptr, name, ext, total_size);
		}
	};


	enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER, TESSELATION_SHADER };

	struct ShaderFile
	{
		string		file_name;
		string		version;
		ShaderType	type;
	};

	struct ShaderProgram
	{
		string		name;
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
		std::vector<string>			texture_keys;

	};

	struct Model
	{
		string				name;
		std::vector<Mesh>	meshes;
	};

	struct CameraPosition
	{
		string	name;

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
		Vec3f	translation;
		float	angle_rotation;
		Vec3f	rotation;
		Vec3f	scale;
	};


	struct InitialModel
	{
		string	model_key;
		string  name;

		MatriceModel transformation;
	};


	struct Scene
	{
		string					name;

		string					skybox_name;

		vector<string>			needed_model;

		CameraPosition			work_camera;
		vector<CameraPosition>	scene_camera;
	};

	struct Environment
	{
		string		name;
		string		root;


		string		active_scene;

		vector<Scene> scenes;

		Environment() : name(), root(), active_scene(), scenes() {

		}
		Environment(string n, string r, string as, vector<Scene> s) : name(n), root(r), active_scene(as), scenes(s) {

		}

		template<typename Ar>
		void serialize(Ar& ar) {
			ar & YAS_OBJECT(nullptr, name, root, active_scene);
		}
	};

}


#endif