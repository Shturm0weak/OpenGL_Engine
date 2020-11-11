#pragma once
#include "../Render/MeshManager.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace Utils {

	static std::string GetNameFromFilePath(const std::string& path, int resolutionLength = 3) {
		int index = path.find_last_of("/");
		return path.substr(index + 1, path.length() - index - 2 - resolutionLength);
	}

	static void LoadMeshesFromFolder(const std::string& path) {
		std::vector<std::string> meshes;
		for (const auto & entry : fs::directory_iterator(path)) {
			meshes.push_back(entry.path().string());
			size_t index = 0;
			index = meshes.back().find("\\", index);
			meshes.back().replace(index, 1, "/");
			Doom::MeshManager::LoadMesh(GetNameFromFilePath(meshes.back()),meshes.back());
		}
	}

	static void LoadShadersFromFolder(const std::string& path) {
		std::vector<std::string> shaders;
		for (const auto & entry : fs::directory_iterator(path)) {
			shaders.push_back(entry.path().string());
			size_t index = 0;
			index = shaders.back().find("\\", index);
			shaders.back().replace(index, 1, "/");
			Doom::Shader::Create(GetNameFromFilePath(shaders.back(),6), shaders.back());
		}
	}

	static void LoadTexturesFromFolder(const std::string& path) {
		std::vector<std::string> textures;
		for (const auto & entry : fs::directory_iterator(path)) {
			std::string pathToTexture = entry.path().string();
			if (pathToTexture.find(".png") <= pathToTexture.length() || pathToTexture.find(".jpeg") <= pathToTexture.length()) {
				textures.push_back(entry.path().string());
				size_t index = 0;
				index = textures.back().find("\\", index);
				textures.back().replace(index, 1, "/");
				Doom::Texture::AsyncLoadTexture(textures.back());
			}
		}
	}

	static glm::vec3 GetPosition(glm::mat4& pos) {
		float* matPtr = glm::value_ptr(pos);
		return glm::vec3(matPtr[12], matPtr[13], matPtr[14]);
	}

	static glm::vec3 GetScale(glm::mat4& scale) {
		float* matPtr = glm::value_ptr(scale);
		return glm::vec3(matPtr[0], matPtr[5], matPtr[10]);
	}
}