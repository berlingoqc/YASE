#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <filesystem>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <flatbuffers/flatbuffers.h>
#include "env_generated.h"

using namespace std;

namespace fs = std::filesystem;

namespace YASE::ENV {


	class ShadersManager
	{
		
	};

	class GroundGenerator
	{
		
	};

	class SkyBoxGenerator 
	{
		

	};

	class EnvironmentLoader
	{
		inline static string folders[5] { "texture", "texture/skybox", "mesh", "asset", "sound" };
		inline static string env_file_name = "environement.fb";

		fs::path			root_folder;
		string				name;

		def::CameraPosition default_position;

		std::vector<def::Texture>	textures_env;
		std::map<int, int>			loaded_textures;


	public:
		EnvironmentLoader()
		{
		}

		bool createNew(fs::path directory,string name) {
			for (const auto& f : folders) {
				fs::path nf = directory / f;
				if(fs::exists(nf))
				{
					return false;
				}
				if (!fs::create_directory(nf))
					return false;
			}
			root_folder = directory;
			this->name = name;
			return save();
		}

		bool save()
		{
			flatbuffers::FlatBufferBuilder builder(512);
			auto n = builder.CreateString(name);
			auto rf = builder.CreateString(root_folder.string());

			auto env = def::CreateEnvironment(builder, n, rf, &default_position);

			builder.Finish(env);

			fs::path fn = root_folder / env_file_name;
			ofstream of(fn.string(), std::ios::binary);
			if (!of.is_open())
				return false;
			of.write((char*)builder.GetBufferPointer(), builder.GetSize());
			of.close();
			return true;
		}

		bool load(fs::path filepath) {
			ifstream inf;
			inf.open(filepath, ios::binary | std::ios::in);
			inf.seekg(0, ios::end);
			int length = inf.tellg();
			inf.seekg(0, ios::beg);
			char* data = new char[length];
			inf.read(data, length);
			inf.close();

			auto env = def::GetEnvironment(data);
			if(env == nullptr)
			{
				return false;
			}
			root_folder = filepath.parent_path();
			name = env->name()->str();
			default_position = *env->camera_default();


			return true;
		}
	};
}

#endif