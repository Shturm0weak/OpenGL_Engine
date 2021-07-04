#pragma once

#include "../Core/Core.h"
#include "Mesh.h"
#include <unordered_map>
#include <map>
#include <mutex>
#include <vector>
#include "OpenGl/VertexArray.h"
#include "OpenGl/VertexBuffer.h"
#include "OpenGl/VertexBufferLayout.h"
#include "OpenGl/IndexBuffer.h"
#include "glm/glm.hpp"
#include <functional>

namespace Doom {

	class DOOM_API Mesh {
	private:

		static std::mutex s_Mtx;
		static std::unordered_map<std::string, Mesh*> s_Meshes;
		static std::multimap<std::string, std::function<void(Mesh* m)>> s_MeshQueue;
		static const char** s_NamesOfMeshes;

		friend class Renderer;
		friend class Editor;
	public:

		float* m_VertAttrib = nullptr;
		glm::vec3 m_TheHighestPoint = glm::vec3(1.0f);
		glm::vec3 m_TheLowestPoint = glm::vec3(-1.0f);
		glm::vec3 m_Color = glm::vec3(1.0f);
		VertexBuffer m_Vb;
		VertexArray m_Va;
		IndexBuffer m_Ib;
		VertexBufferLayout m_Layout;
		std::string m_Name;
		std::string m_FilePath;
		bool m_IsInitialized = false;
		uint32_t m_IdOfMeshInFile = 0;
		uint32_t m_VertAttribSize = 0;
		uint32_t m_IndicesSize = 0;
		uint32_t* m_Indices = nullptr;

		Mesh(std::string name, std::string filePath) : m_Name(name), m_FilePath(filePath) {}
		~Mesh();

		static int GetAmountOfMeshes();
		static void LoadMesh(std::string name, std::string filePath, size_t meshId = 0);
		static void LoadScene(std::string filepath);
		static void AsyncLoad(std::string name, std::string filePath, size_t meshId = 0);
		static Mesh* Get(std::string name);
		static const char** GetListOfMeshes();
		static void Add(Mesh* mesh);
		static void AsyncGet(std::function<void(Mesh* m)> function,std::string name);
		static void Delete(std::string name);
		static void Delete(Mesh* mesh);
		static void ShutDown();
		static void DispatchLoadedMeshes();

		void Init();
		void Refresh();
	};

}