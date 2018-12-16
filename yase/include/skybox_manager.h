#ifndef SKYBOX_MANAGER_H
#define SKYBOX_MANAGER_H

#include "base_manager.h"
#include "textures.h"
#include "logger.h"

typedef std::map<std::string, std::tuple<SkyBox, uint>> MapSkyBox;

class SkyBoxManager : public AssetManager
{
	inline const static std::string									skybox_files[6]{ "right", "left", "top", "bot", "front", "back" };

	ENGINE::MyTexture												texture_loader;
	MapSkyBox														boxes;
	int																loaded_skybox_index = 0;
	
public:


	template<typename Ar>
	void serialize(Ar& ar) {
		for (auto& b : boxes) {
			std::get<1>(b.second) = ERROR_TEXTURE;
		}
		ar & YAS_OBJECT(nullptr, boxes);
	}


	std::vector<const char*> getKeys()
	{
		std::vector<const char*> v;
		for (const auto& t : boxes)
			v.emplace_back(t.first.c_str());
		return v;
	}

	virtual void saveManager(std::ostream* writer) {

	}

	virtual void loadManager(std::ifstream* reader) {

	}

	SkyBoxManager();

	int getNumberSkyBox() const
	{
		return boxes.size();
	}

	int getNumberLoadedSkyBox() const
	{
		return loaded_skybox_index;
	}


	const MapSkyBox& getMapSkyBox() const
	{
		return boxes;
	}

	bool isLoaded(std::string key);

	bool AddSkyBox(const std::string& name, std::vector<char*> faces,std::vector<int> sizes);

	bool AddSkyBox(const std::string& name, const fs::path& directory);


	uint LoadSkyBox(std::string name);

};

#endif