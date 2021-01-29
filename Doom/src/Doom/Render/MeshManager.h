#pragma once
#include <iostream>
#include "../Core/Core.h"
#include "Mesh.h"
#include <map>
#include <mutex>

namespace Doom {

	class DOOM_API MeshManager {
	private:

		MeshManager& operator=(const MeshManager& rhs) { return *this; }
		MeshManager(const MeshManager&) = delete;
		MeshManager() {}

		std::mutex s_Mtx;
		std::map<std::string, Mesh*> s_Meshes;
		std::vector <Mesh*> s_NeedToInitMeshes;
		std::multimap<std::string, void*> s_MeshQueue;
		const char** s_NamesOfMeshes;
	public:

		static MeshManager& GetInstance();
		int GetAmountOfMeshes();
		void LoadMesh(std::string name, std::string filepath, uint32_t meshId = 0);
		void LoadScene(std::string filepath);
		void AsyncLoadMesh(std::string name, std::string filepath, uint32_t meshId = 0);
		Mesh* GetMesh(std::string name);
		const char** GetListOfMeshes();
		void AddMesh(Mesh* mesh);
		void GetMeshWhenLoaded(std::string name, void* r);
		void DeleteMesh(std::string name);
		void DeleteMesh(Mesh* mesh);
		void ShutDown();
		void DispatchLoadedMeshes();

		friend class Renderer;
		friend class Editor;
	};

}