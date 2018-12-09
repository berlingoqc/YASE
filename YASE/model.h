#ifndef MODEL_H
#define MODEL_H


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
	};

	struct TextureOption
	{
		TexWrappingOptions	wrapping;
		TexFilterOptions	filter;
	};


	struct SkyBox
	{
		string		name;
		string		ext;

		int			total_size;
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
	};

	struct Vec2f
	{
		float x;
		float y;
	};

	struct Vertex
	{

		Vec3f	position;
		Vec3f	normal;
		Vec2f	texcoord;

		Vec3f	tangeant;
		Vec3f	bitangeant;

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
	};

}


#endif