#ifndef MESH_H
#define MESH_H

#include <vector>
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include <iostream>
#include "../Core/Core.h"

namespace Doom {

	class DOOM_API Mesh {
	public:

		std::string name;
		uint32_t meshSize = 0;
		float* mesh = nullptr;
		uint32_t indicesSize = 0;
		uint32_t* indicesForNormals = nullptr;

		float* verteces = nullptr;
		uint32_t vertecesSize = 0;
		float* vertecesForNormals = nullptr;
		uint32_t vertecesSizeForNormals = 0;
		uint32_t indicesSizeForNormals = 0;
		uint32_t* indices = nullptr;
		float* normals = nullptr;
		uint32_t normalsSize = 0;
		uint32_t uvSize = 0;
		float* uv;
		uint32_t uvSizeForVert = 0;
		float* uvForVert;
		uint32_t uvIndexSize = 0;
		uint32_t* uvIndex;
		float* btangent;
		float* tangent;

		void Init();

		VertexBuffer* vb = nullptr;
		VertexArray* va = nullptr;
		IndexBuffer* ib = nullptr;
		VertexBufferLayout* layout = nullptr;

		Mesh(std::string name) : name(name) {}

		~Mesh();
	};

}
#endif // MESH_H