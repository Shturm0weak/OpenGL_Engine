#pragma once

#include <filesystem>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"

namespace fs = std::filesystem;

namespace Utils {

	static void SetStandardTexParams()
	{
		Doom::Texture::s_TexParameters.resize(4);
		Doom::Texture::s_TexParameters[0] = { GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR };
		Doom::Texture::s_TexParameters[1] = { GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR };
		Doom::Texture::s_TexParameters[2] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE };
		Doom::Texture::s_TexParameters[3] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE };
	}

	static std::string GetNameFromFilePath(const std::string& path, int resolutionLength = 3)
	{
		int index = path.find_last_of("/");
		if (index == -1)
			index = path.find_last_of("\\");
		return path.substr(index + 1, path.length() - index - 2 - resolutionLength);
	}

	static void LoadMeshesFromFolder(const std::string& path)
	{
		std::vector<std::string> meshes;
		for (const auto & entry : fs::directory_iterator(path))
		{
			meshes.push_back(entry.path().string());
			size_t index = 0;
			index = meshes.back().find("\\", index);
			meshes.back().replace(index, 1, "/");
			Doom::Mesh::LoadMesh(GetNameFromFilePath(meshes.back()),meshes.back());
		}
	}

	static void LoadShadersFromFolder(const std::string& path) 
	{
		std::vector<std::string> shaders;
		for (const auto & entry : fs::directory_iterator(path))
		{
			shaders.push_back(entry.path().string());
			size_t index = 0;
			index = shaders.back().find("\\", index);
			shaders.back().replace(index, 1, "/");
			Doom::Shader::Create(GetNameFromFilePath(shaders.back(),6), shaders.back());
		}
	}

	static void LoadTexturesFromFolder(const std::string& path)
	{
		std::vector<std::string> textures;
		for (const auto & entry : fs::directory_iterator(path)) 
		{
			std::string pathToTexture = entry.path().string();
			if (pathToTexture.find(".png") <= pathToTexture.length() || pathToTexture.find(".jpeg") <= pathToTexture.length())
			{
				textures.push_back(entry.path().string());
				size_t index = 0;
				index = textures.back().find("\\", index);
				textures.back().replace(index, 1, "/");
				Doom::Texture::AsyncLoadTexture(textures.back());
			}
		}
	}

	static std::vector<std::string> GetFilesName(const std::string& path, const char* filter)
	{
		std::vector<std::string> files;
		for (const auto& entry : fs::directory_iterator(path)) 
		{
			std::string pathToTexture = entry.path().string();
			if (pathToTexture.find(".png") <= pathToTexture.length() || pathToTexture.find(".jpeg") <= pathToTexture.length()) {
				files.push_back(entry.path().string());
			}
		}
		return files;
	}

	static glm::vec3 GetPosition(glm::mat4& pos) 
	{
		float* matPtr = glm::value_ptr(pos);
		return std::move(glm::vec3(matPtr[12], matPtr[13], matPtr[14]));
	}

	static glm::vec3 GetScale(glm::mat4& scale) 
	{
		float* matPtr = glm::value_ptr(scale);
		return std::move(glm::vec3(matPtr[0], matPtr[5], matPtr[10]));
	}

	static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
	{
		// From glm::decompose in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>())) return false;

		// First, isolate perspective.  This is the messiest.
		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0)
		{
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else
		{
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}
		return true;
	}

	template<class T>
	char* PreAllocateMemory(std::map<char*, uint64_t>& memoryPool, std::vector<char*>& freeMemory)
	{
		std::map<char*, uint64_t>::iterator iter;
		
		if (memoryPool.size() == 0 || memoryPool.rbegin()->second == MAX_PREALLOCATED_INSTANCES)
		{
			char* newPreAllocMemory = (char*)((void*)(new T[MAX_PREALLOCATED_INSTANCES]));
			memoryPool.insert(std::make_pair(newPreAllocMemory, 0));
			char* memoryPtr = memoryPool.rbegin()->first;
			iter = memoryPool.find(memoryPtr);
			for (size_t i = 0; i < MAX_PREALLOCATED_INSTANCES * sizeof(T); i += sizeof(T))
			{
				freeMemory.push_back(newPreAllocMemory + i);
			}
		}
		if (freeMemory.size() > 0)
		{
			for (auto iterMP = memoryPool.begin(); iterMP != memoryPool.end(); iterMP++)
			{
				uint64_t ptr = (uint64_t)freeMemory.back();
				if ((uint64_t)(iterMP->first) <= ptr && ptr <= (uint64_t)(iterMP->first) + MAX_PREALLOCATED_INSTANCES * sizeof(T))
				{
					iterMP->second++;
					char* ptr = freeMemory.back();
					freeMemory.pop_back();
					return ptr;
				}
			}
		}
		return nullptr;
	}

	template<typename T>
	static std::string GetComponentTypeName()
	{
		std::string name = typeid(T).name();
		name = (name.substr(6));
		return (name);
	}

}