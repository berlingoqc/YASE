#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H


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

namespace fs = std::filesystem;
using namespace std;


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
	m.set_name("ground");
	YASE::DEF::Mesh mesh;
	for(int i = 0; i < 4; i++)
	{
		YASE::DEF::Vec3f* vert = new YASE::DEF::Vec3f();
		YASE::DEF::Vec2f* tex = new YASE::DEF::Vec2f();

		vert->set_x(vertex_base[i].x);
		vert->set_y(vertex_base[i].y);
		vert->set_z(vertex_base[i].z);

		tex->set_x(tex_bas[i].x);
		tex->set_y(tex_bas[i].y);

		auto* v = mesh.add_vertices();
		v->set_allocated_position(vert);
		v->set_allocated_texcoord(tex);
	}
	for(int i = 0;i<6;i++)
	{
		mesh.add_indices(indice_base[i]);
	}
	mesh.add_textures_index(0);
	auto* am = m.add_meshes();
	*am = mesh;

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

	YaseMesh(std::vector<Vertex> vert, std::vector<uint> indices)
	{
		this->vertices = vert;
		have_ibo = !indices.empty();
		this->indices = indices;
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

	std::vector<YaseMesh>	meshes;

public:

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

	// Crée les buffer gpu et charge les textures requis depuis le texturemanager
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



class ModelManager : public AssetManager
{
	inline const static string		mesh_folder[2]{ "yase", "obj" };

	TextureManager*					tex_manager;

	std::map<string, YaseModel>		map_model;


public:
	ModelManager()
	{
		
	}


	void Save()
	{
		
	}


	void Load()
	{
		
	}

	void AddModel(string name, YaseModel model)
	{
		
	}

	YaseModel*	getLoadedModel(string name)
	{

		return nullptr;
	}


};


#endif