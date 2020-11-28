#pragma once
#include <iostream>
#include "../Core/Core.h"
#include "Mesh.h"
#include <map>

namespace Doom {

	class DOOM_API MeshManager {
	private:
		static std::map<std::string, Mesh*> Meshes;
		static std::vector <Mesh*> needToInitMeshes;
		static std::multimap<std::string, void*> meshQueue;
		static const char** listOfMeshes;
	public:
		static void LoadMesh(std::string name, std::string filepath, uint32_t meshId = 0);

		static void LoadScene(std::string filepath);

		static void AsyncLoadMesh(std::string name, std::string filepath, uint32_t meshId = 0);

		static Mesh* GetMesh(std::string name);

		static const char** GetListOfMeshes();

		static void AddMesh(Mesh* mesh);

		inline static int GetAmountOfMeshes() { return Meshes.size(); }

		static void GetMeshWhenLoaded(std::string name, void* r);

		static void DeleteMesh(std::string name);

		static void DeleteMesh(Mesh* mesh);

		static void ShutDown();

		static void DispatchLoadedMeshes();

		friend class Renderer;
		friend class Editor;
	};

}