#pragma once

namespace Utils {

	DOOM_API void LoadMeshesFromFolder(const std::string& path);
	DOOM_API void LoadShadersFromFolder(const std::string& path);
	DOOM_API void LoadTexturesFromFolder(const std::string& path);
	DOOM_API std::string GetNameFromFilePath(const std::string& path, int resolutionLength = 3);
	DOOM_API std::vector<std::string> GetFilesName(const std::string& path, const char* filter);
	DOOM_API glm::vec3 GetPosition(glm::mat4& pos);
	DOOM_API glm::vec3 GetScale(glm::mat4& scale);
	DOOM_API void SetStandardTexParams();
	DOOM_API bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

	DOOM_API glm::dvec3 Normalize(glm::dvec3 vector);
	DOOM_API glm::vec3 Normalize(glm::vec3 vector);
	DOOM_API glm::vec2 Normalize(glm::vec2 vector);
	DOOM_API float Distance(glm::vec2 start, glm::vec2 end);
	DOOM_API float Distance(glm::dvec3 start, glm::dvec3 end);
	DOOM_API float Distance(glm::vec3 start, glm::vec3 end);

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

	template<class T>
	std::string GetComponentTypeName()
	{
		return (std::string(typeid(T).name()).substr(6));
	}

}