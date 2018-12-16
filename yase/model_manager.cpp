#include "model_manager.h"

YaseMesh::YaseMesh() { }
YaseMesh::YaseMesh(std::vector<Vertex> vert, std::vector<uint> indices, std::vector<std::string> texture)
	{
		this->vertices = vert;
		this->indices = indices;
		this->needed_texture = texture;
	}


	void YaseMesh::Allocate()
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

	void YaseMesh::Draw(uint shader)
	{
		uint diffuseNbr = 1;
		static const std::string name = "texture_diffuse";
		for(uint i = 0; i < textures.size();i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			std::string n = name + std::to_string(i);
			glUniform1i(glGetUniformLocation(shader, "ourTexture1"), i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
		}
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}


void YaseModel::Draw(uint shader)
	{
		for(auto& m : meshes)
		{
			m.Draw(shader);
		}
		
	}


	bool YaseModel::ReadModelInfoAssimp(fs::path filename,TextureManager* tm)
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
	void YaseModel::processNode(aiNode* node, const aiScene* scene,TextureManager* tm)
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

	YaseMesh YaseModel::processMesh(aiMesh* mesh, const aiScene* scene,TextureManager* tm)
	{
		// data to fill
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<std::string> textures;

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			Vec3f vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// normals
			if (mesh->mNormals != nullptr) {
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			}
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				Vec2f vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = { 0.0f, 0.0f };

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
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<std::string> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse",tm);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		}

		return YaseMesh(vertices, indices,textures);
	}

	std::vector<std::string> YaseModel::loadMaterialTextures(aiMaterial* mat,aiTextureType type, std::string typeName,TextureManager* tm)
	{
		std::vector<std::string> textures;
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
					// Cree la load texture avec notre info
					std::string name = this->name + text_path.filename().string();
					tm->AddNewLoadedTexture(name, text_path.filename().string(),REPEATED,LINEAR);
					textures.emplace_back(name);
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
	void YaseModel::LoadModel(TextureManager* tm)
	{
		std::vector<uint> tid;
		for(auto& m : meshes)
		{
			for (const auto& s : m.needed_texture) {
				printf("%s\n", s.c_str());
			}
			tid = tm->loadTexture(m.needed_texture);
			m.setTextures(tid);
			m.Allocate();
			tid.clear();
		}
	}
	
	void ModelManager::generateMap() {
		for (const auto& k : keys) {
			map_model[k] = nullptr;
		}
	}


	ModelManager::ModelManager()
	{
		file_name = "model.yase";
		
	}

	// Valide que tous les models present dans le fichier exists dans le repertoire
	void ModelManager::ValidIntegrity()
	{
		
	}

	void ModelManager::ImportModel(std::string name, fs::path filepath)
	{
		
	}
	// Charge depuis la memoire les informations du model (meshes)
	void ModelManager::ReadModelInfo(std::string key, fs::path fullpath,YaseModel* ym)
	{
		// TODO : Valide que fullpath existe bien
		yas::file_istream ifs(fullpath.string().c_str());
		yas::load<flags_yas_bf>(ifs, *ym);
	}

	// Import un model d'un autre type de fichier dans mon mode de fichier avec ce qu'assimp supporte
	void ModelManager::AddModelAssimp(std::string name, fs::path filepath)
	{
		if(name.empty())
		{
			YASE_LOG_ERROR(("Aucune nom donner pour le model importer"));
			return;
		}
		YaseModel* m = new YaseModel();
		m->name = name;
		m->ReadModelInfoAssimp(filepath, tex_manager);
		m->LoadModel(tex_manager);
		map_model[name] = m;
		keys.emplace_back(name);

		fs::path fp = root_folder / name;
		yas::file_ostream of(fp.string().c_str(), yas::file_trunc);
		yas::save<flags_yas_bf>(of, *m);
		of.flush();
	}

	void ModelManager::AddModel(std::string name, YaseModel ym) {
		if (name.empty())
		{
			YASE_LOG_ERROR(("Aucune nom donner pour le model importer"));
			return;
		}		
		YaseModel* m = new YaseModel();
		*m = ym;
		map_model[name] = m;
		keys.emplace_back(name);
		m->LoadModel(tex_manager);
		index_loaded_model++;
	}

	// Ajout un yasemodel existant dans la collection. Le sauvegarde tout de suite. C'est la fonction qui s'occupe de ca
	void ModelManager::AddModel(std::string name, Model model)
	{
		// Regarde qu'il existe pas deja
		const auto& v = map_model.find(name);
		if (v != map_model.end())
			throw ModelException(true, name);
		name = name.append(extension_model);
		fs::path fp = root_folder / name;
		std::ofstream of(fp.string(), std::ios::binary);
		if (!of.is_open()) {
			YASE_LOG_ERROR(("Impossible d'oubrir le fichier " + fp.string()).c_str());
			return;
		}

		of.close();
	
		map_model[name] = nullptr;
		// devrait essayer de le loader tout suite quand on l'ajoute
	}

	// Retourne le pointeur d'un model deja loader. ( Le load si besoin )
	YaseModel*	ModelManager::getLoadedModel(std::string name)
	{
		const auto& v = map_model.find(name);
		if (v == map_model.end()) {
			// Model n'existe pas
			//throw ModelException(false, name);
			YASE_LOG_ERROR(("La cle du model a charger n'existe pas " + name).c_str());
			return nullptr;
		}
		if(v->second == nullptr) {
			YaseModel* ym = new YaseModel();
			//name = name.append(extension_model);
			fs::path fp = root_folder / name;
			ReadModelInfo(name,fp,ym);
			ym->LoadModel(tex_manager);
			index_loaded_model++;
			v->second = ym;
			return ym;
		}
		return v->second;
	}

	// Efface le model de la memoire de gpu ( quand on n'a plus besoin , faudrait je garde le nombre de reference )
	void ModelManager::releaseModel(std::string name)
	{
		
	}
