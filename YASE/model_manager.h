#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <filesystem>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "logger.h"
#include "texture.pb.h"
#include "mesh.pb.h"
#include "textures.h"

#include "base_manager.h"
#include "texture_manager.h"
#include "mesh.h"
#include "mesh.pb.h"

namespace fs = std::filesystem;
using namespace Assimp;
using namespace std;

static float largeur = 8.0f;
static float hauteur = 30.0f;
static float profondeur = 2.5f;

static float profondeur_r = 2.5f;
static float hauteur_r = 10.0f;

static glm::vec3    vertex_base_home[12]
{
glm::vec3(-largeur, hauteur, -profondeur),
glm::vec3(largeur, hauteur, -profondeur),
glm::vec3(-largeur, -hauteur, -profondeur),
glm::vec3(largeur, -hauteur, -profondeur),
glm::vec3(-largeur, hauteur, profondeur),
glm::vec3(largeur, hauteur, profondeur),
glm::vec3(-largeur, -hauteur, profondeur),
glm::vec3(largeur, -hauteur, profondeur),
glm::vec3(largeur / 3, -hauteur, -profondeur - 0.001),
glm::vec3(-largeur / 3, -hauteur, -profondeur - 0.001),
glm::vec3(-largeur / 3, hauteur / 2, -profondeur - 0.001),
glm::vec3(largeur / 3, hauteur / 2, -profondeur - 0.001)
};

static glm::vec3	vertex_roof_home[6]
{
	glm::vec3(0.0, hauteur + hauteur_r, -profondeur_r),
	glm::vec3(-largeur, hauteur, -profondeur_r),
	glm::vec3(largeur, hauteur, -profondeur_r),
	glm::vec3(0.0, hauteur + hauteur_r, profondeur_r),
	glm::vec3(-largeur, hauteur, profondeur_r),
	glm::vec3(largeur, hauteur, profondeur_r)
};

static unsigned int	indice_base_home[36]
{
0,1,2,
1,2,3,
4,5,6,
5,6,7,
2,6,7,
2,3,7,
0,4,5,
0,1,5,
1,5,7,
1,3,7,
0,2,4,
2,4,6
};

static unsigned int	indice_roof_home[18]
{
0,1,2,
3,4,5,
0,5,2,
0,3,5,
0,4,1,
0,3,4
};

static glm::vec2	texture_roof_home[6]
{
	glm::vec2(0.0f, 1.0f),
	glm::vec2(0.0f, 0.0f),
	glm::vec2(0.0f, 0.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(1.0f, 0.0f)
};

static glm::vec2	texture_base_home[8]
{
	glm::vec2(0.0f, 1.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(0.0f, 0.0f),
	glm::vec2(1.0f, 0.0f),

	glm::vec2(1.0f, 1.0f),
	glm::vec2(0.0f, 1.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(0.0f, 0.0f)
};

static glm::vec3	vertex_base[4]
{
	{-500,0,-500}, {500,0,-500}, {-500,0,500}, {500,0,500}
};
static unsigned int	indice_base[6]
{
	0,1,2,1,2,3
};
static glm::vec2	tex_bas[4]
{
	{0.0,0.0}, {100,0}, {0,100}, {100,100}
};


inline YASE::DEF::Model getDefaultModel()
{
	YASE::DEF::Model m;
	m.set_name("house");
	YASE::DEF::Mesh mesh;
	for(int i = 0; i < 12; i++)
	{
		YASE::DEF::Vec3f* vert = new YASE::DEF::Vec3f();
		YASE::DEF::Vec2f* tex = new YASE::DEF::Vec2f();

		vert->set_x(vertex_base_home[i].x);
		vert->set_y(vertex_base_home[i].y);
		vert->set_z(vertex_base_home[i].z);
		if (i < 8) {
			tex->set_x(texture_base_home[i].x);
			tex->set_y(texture_base_home[i].y);
		}

		auto* v = mesh.add_vertices();
		v->set_allocated_position(vert);
		v->set_allocated_texcoord(tex);
	}
	for(int i = 0;i<36;i++)
	{
		mesh.add_indices(indice_base_home[i]);
	}
	mesh.add_textures_index(1);
	auto* am = m.add_meshes();
	*am = mesh;

	mesh = YASE::DEF::Mesh();
	for (int i = 0; i < 18; i++)
	{
		YASE::DEF::Vec3f* vert = new YASE::DEF::Vec3f();
		YASE::DEF::Vec2f* tex = new YASE::DEF::Vec2f();

		vert->set_x(vertex_roof_home[i].x);
		vert->set_y(vertex_roof_home[i].y);
		vert->set_z(vertex_roof_home[i].z);

		if (i < 6) {
			tex->set_x(texture_roof_home[i].x);
			tex->set_y(texture_roof_home[i].y);
		}

		auto* v = mesh.add_vertices();
		v->set_allocated_position(vert);
		v->set_allocated_texcoord(tex);
	}
	for (int i = 0; i < 6; i++)
	{
		mesh.add_indices(indice_base[i]);
	}
	mesh.add_textures_index(2);
	auto* rm = m.add_meshes();
	*rm = mesh;
	return m;
}



class YaseMesh
{
	bool					have_ibo = true;
	bool					have_tangant = false;
	bool					have_bitangant = false;

	uint					VAO = 0 , VBO = 0 , EBO= 0 ;

public:
	std::vector<Vertex>		vertices;
	std::vector<uint>		indices;
	std::vector<uint>		textures;

	std::vector<int>		needed_texture;

	YaseMesh(std::vector<Vertex> vert, std::vector<uint> indices, std::vector<int> texture)
	{
		this->vertices = vert;
		this->indices = indices;
		this->needed_texture = texture;
	}

	YaseMesh(const YASE::DEF::Mesh& m)
	{
		for(int i = 0 ; i < m.vertices_size(); i++)
		{
			Vertex		vertex;
			glm::vec3	vector;
			const auto&  v = m.vertices(i);
			vector.x = v.position().x();
			vector.y = v.position().y();
			vector.z = v.position().z();
			vertex.Position = vector;
			vector.x = v.normal().x();
			vector.y = v.normal().y();
			vector.z = v.normal().z();
			vertex.Normal = vector;
			// texture coordinates
			if (v.has_texcoord()) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = v.texcoord().x();
				vec.y = v.texcoord().y();
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}
		for(int i = 0; i < m.indices_size();i++)
		{
			const auto& v = m.indices(i);
			indices.emplace_back(v);
		}
		for(int i = 0;i<m.textures_index_size();i++)
		{
			const auto& v = m.textures_index(i);
			needed_texture.emplace_back(v);
		}
	}



	const vector<int>& getNeededTexture() const { return needed_texture; }

	void setTextures(std::vector<uint> text)
	{
		textures = text;
	}

	void Allocate()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);
		
		// vertex position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangeant
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangeant
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);
	}

	void Draw(uint shader)
	{
		uint diffuseNbr = 1;
		static const std::string name = "texture_diffuse";
		for(uint i = 0; i < textures.size();i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			string n = name + std::to_string(i);
			glUniform1i(glGetUniformLocation(shader, n.c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
		}
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}

};

class YaseModel
{
	string					name;
	bool					has_been_update = false;

	std::vector<YaseMesh>	meshes;

	fs::path				directory;

public:
	
	bool hasBeenUpdate() { return has_been_update; }

	// Draw dessine le model avec le shader donner
	void Draw(uint shader)
	{
		for(auto& m : meshes)
		{
			m.Draw(shader);
		}
		
	}

	void LoadFromDefModel(YASE::DEF::Model& model)
	{
		name = model.name();
		for (int i = 0; i < model.meshes_size(); i++)
		{
			const auto& m = model.meshes(i);
			YaseMesh ym(m);
			meshes.emplace_back(m);
		}

	}

	// Charge depuis la memoire les informations du model (meshes)
	bool ReadModelInfo(fs::path fullpath)
	{
		ifstream inf(fullpath.string(), ios::binary | std::ios::in);
		if (!inf.is_open())
			return false;
		YASE::DEF::Model model;
		if (!model.ParseFromIstream(&inf))
			return false;
		LoadFromDefModel(model);
		return true;
	}

	bool ReadModelInfoAssimp(fs::path filename,TextureManager* tm)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename.string(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return false;
		}
		directory = filename.parent_path();
		processNode(scene->mRootNode, scene, tm);
		return true;
	}
	void processNode(aiNode* node, const aiScene* scene,TextureManager* tm)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh,scene,tm));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene,tm);
		}
	}

	YaseMesh processMesh(aiMesh* mesh, const aiScene* scene,TextureManager* tm)
	{
		// data to fill
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<int> textures;

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// tangent
			/*vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;*/

			vertices.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		for (uint i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* material = scene->mMaterials[i];
			std::vector<int> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse",tm);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		}

		return YaseMesh(vertices, indices,textures);
	}

	vector<int> loadMaterialTextures(aiMaterial* mat,aiTextureType type, std::string typeName,TextureManager* tm)
	{
		std::vector<int> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			if (mat->GetTexture(type, i, &str) == AI_SUCCESS) {
				// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
				// if texture hasn't been loaded already, load it
				fs::path text_path = str.C_Str();
				text_path = directory / text_path;
				if(tm->AddNewTexture(text_path))
				{
					textures.emplace_back(tm->getCurrentIndex() - 1);
				}
			}
			else
			{
				printf("Failed load %s\n", typeName.c_str());
			}
		}
		return textures;
	}


	// Cr�e les buffer gpu et charge les textures requis depuis le texturemanager
	void LoadModel(TextureManager* tm)
	{
		vector<uint> tid;
		for(auto& m : meshes)
		{
			tid = tm->loadTexture(m.getNeededTexture());
			m.setTextures(tid);
			m.Allocate();
		}
	}
	
};

struct ModelException : public std::exception {

	bool exists;
	string name;

	ModelException(bool e, string n) {
		exists = e;
		name = n;
	}

	const char* what() const throw() {
		if (exists)
			return (("Le model exists deja " + name).c_str());
		return (("Le model n'existe pas " + name).c_str());
	}
};




class ModelManager : public AssetManager
{
	inline const static string		extension_model = ".model";
	inline const static string		mesh_folder[2]{ "yase", "obj" };


	// Une map qui contient tous mes models que je possede
	// dans mon arborscence. La liste est loader au demarrage
	// et les assets du model sont loader quand on n'a besoin
	// et les textures sont entreposer dans TextureManager
	std::map<string, YaseModel*>		map_model;
	int									index_loaded_model = 0;


public:
	TextureManager*					tex_manager;
	vector<const char*> getKeys()
	{
		vector<const char*> v;
		for (const auto& t : map_model)
			v.emplace_back(t.first.c_str());
		return v;
	}
	virtual void saveManager(ostream* writer) {
		YASE::DEF::ModelList ml;
		for (const auto& m : map_model) {
			// TODO : Regarde si un model a ete update si oui on ecrase l'ancien fichier
			auto* model = m.second;
			ml.add_models(m.first);
		}
		if (!ml.SerializeToOstream(writer)) {

		}
	}

	virtual void loadManager(ifstream* reader) {
		YASE::DEF::ModelList ml;
		if (!ml.ParseFromIstream(reader))
		{
			
		}
		for (int i = 0; i < ml.models_size(); i++)
		{
			map_model[ml.models(i)] = nullptr;
		}
		ValidIntegrity();
	}

	ModelManager()
	{
		file_name = "model.yase";
		
	}

	const std::map<string, YaseModel*>&	getModelMap() {
		return map_model;
	}

	int getNumberModel() {
		return map_model.size();
	}

	int getNumberLoadedModel() {
		return index_loaded_model;
	}


	// Valide que tous les models present dans le fichier exists dans le repertoire
	void ValidIntegrity()
	{
		
	}

	// Import un model d'un autre type de fichier dans mon mode de fichier avec ce qu'assimp supporte
	void ImportModel(string name, fs::path filepath)
	{
		
	}

	void AddModelAssimp(string name, fs::path filepath)
	{
		YaseModel* m = new YaseModel();
		m->ReadModelInfoAssimp(filepath, tex_manager);
		m->LoadModel(tex_manager);
		map_model[name] = m;

	}

	// Ajout un yasemodel existant dans la collection. Le sauvegarde tout de suite. C'est la fonction qui s'occupe de ca
	void AddModel(string name, YASE::DEF::Model model)
	{
		// Regarde qu'il existe pas deja
		const auto& v = map_model.find(name);
		if (v != map_model.end())
			throw ModelException(true, name);
		name = name.append(extension_model);
		fs::path fp = root_folder / name;
		ofstream of(fp.string(), ios::binary);
		if (!of.is_open()) {
			return;
		}
		if (!model.SerializePartialToOstream(&of)) {
			return;
		}
		of.close();
		map_model[name] = nullptr;
		// devrait essayer de le loader tout suite quand on l'ajoute
	}

	// Retourne le pointeur d'un model deja loader. ( Le load si besoin )
	YaseModel*	getLoadedModel(string name)
	{
		const auto& v = map_model.find(name);
		if (v == map_model.end()) {
			// Model n'existe pas
			throw ModelException(false, name);
		}
		if(v->second == nullptr) {
			YaseModel* ym = new YaseModel();
			//name = name.append(extension_model);
			fs::path fp = root_folder / name;
			if (!ym->ReadModelInfo(fp)) {
				return nullptr;
			}
			v->second = ym;
			v->second->LoadModel(tex_manager);
		}
		return v->second;
	}

	// Efface le model de la memoire de gpu ( quand on n'a plus besoin , faudrait je garde le nombre de reference )
	void releaseModel(string name)
	{
		
	}


};


#endif