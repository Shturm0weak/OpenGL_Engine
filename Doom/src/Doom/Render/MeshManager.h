#pragma once
#include <iostream>
#include "../Core/Core.h"
#include "Mesh.h"
#include <map>
#include <mutex>

namespace Doom {

	class DOOM_API MeshManager {
	private:

		static std::mutex s_Mtx;
		static std::map<std::string, Mesh*> s_Meshes;
		static std::vector <Mesh*> s_NeedToInitMeshes;
		static std::multimap<std::string, void*> s_MeshQueue;
		static const char** s_NamesOfMeshes;
	public:

		inline static int GetAmountOfMeshes() { return s_Meshes.size(); }
		static void LoadMesh(std::string name, std::string filepath, uint32_t meshId = 0);
		static void LoadScene(std::string filepath);
		static void AsyncLoadMesh(std::string name, std::string filepath, uint32_t meshId = 0);
		static Mesh* GetMesh(std::string name);
		static const char** GetListOfMeshes();
		static void AddMesh(Mesh* mesh);
		static void GetMeshWhenLoaded(std::string name, void* r);
		static void DeleteMesh(std::string name);
		static void DeleteMesh(Mesh* mesh);
		static void ShutDown();
		static void DispatchLoadedMeshes();

		friend class Renderer;
		friend class Editor;
	};

}