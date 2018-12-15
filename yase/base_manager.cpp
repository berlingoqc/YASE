#include "base_manager.h"

	void AssetManager::ValidFile(const fs::path& fn) {
		if (root_folder.empty()) {
			FileDontExist e;
			e.file = fn.string();
			throw e;
		}
	}

	void AssetManager::Save() {
		const fs::path fn = getFileFullPath();
		ValidFile(fn);
		std::ofstream of(getFileFullPath(), std::ios::binary);
		saveManager(&of);
		of.close();
	}

	void AssetManager::Load() {
		const fs::path fn = getFileFullPath();
		ValidFile(fn);
		std::ifstream inf(fn.string(), std::ios::binary | std::ios::in);
		loadManager(&inf);
		inf.close();
	}

	void AssetManager::setRootFolder(fs::path rf)
	{
		root_folder = rf;
	}

	fs::path AssetManager::getFileFullPath() const
	{
		return (root_folder / file_name);
	}