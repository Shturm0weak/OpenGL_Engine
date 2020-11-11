#ifndef MESH_H
#define MESH_H

#include <vector>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include <iostream>
#include "../Core/Core.h"
#include "glm/glm.hpp"

namespace Doom {

	class DOOM_API Mesh {
	public:

		std::string name;
		std::string filePath;
		uint32_t meshSize = 0;
		float* mesh = nullptr;
		uint32_t indicesSize = 0;
		uint32_t* indicesForNormals = nullptr;
		glm::vec3 theHighestPoint = glm::vec3(1.0f);
		glm::vec3 theLowestPoint = glm::vec3(-1.0f);
		float* verteces = nullptr;
		uint32_t vertecesSize = 0;
		float* vertecesForNormals = nullptr;
		uint32_t vertecesSizeForNormals = 0;
		uint32_t indicesSizeForNormals = 0;
		uint32_t* indices = nullptr;
		float* normals = nullptr;
		uint32_t normalsSize = 0;
		uint32_t uvSize = 0;
		float* uv = nullptr;
		uint32_t uvSizeForVert = 0;
		float* uvForVert = nullptr;
		uint32_t uvIndexSize = 0;
		uint32_t* uvIndex;
		float* btangent = nullptr;
		float* tangent = nullptr;

		void Init();

		VertexBuffer* vb = nullptr;
		VertexArray* va = nullptr;
		IndexBuffer* ib = nullptr;
		VertexBufferLayout* layout = nullptr;

		Mesh(std::string name, std::string filePath) : name(name), filePath(filePath) {}

		~Mesh();
	};

}
#endif