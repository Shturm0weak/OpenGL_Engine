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

		std::string name;
		std::string filePath;
		float* mesh = nullptr;
		uint32_t meshSize = 0;
		uint32_t indicesSize = 0;
		uint32_t* indices = nullptr;
		glm::vec3 theHighestPoint = glm::vec3(1.0f);
		glm::vec3 theLowestPoint = glm::vec3(-1.0f);

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