#ifndef MESH_H
#define MESH_H

#include <vector>
#include "OpenGl/VertexArray.h"
#include "OpenGl/VertexBuffer.h"
#include "OpenGl/VertexBufferLayout.h"
#include "OpenGl/IndexBuffer.h"
#include <iostream>
#include "../Core/Core.h"
#include "glm/glm.hpp"

namespace Doom {

	class DOOM_API Mesh {
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

		void Init();
		void Refresh();
	};

}
#endif