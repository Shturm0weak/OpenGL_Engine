#ifndef MESH_H
#define MESH_H

#include <iostream>
#include "../Core/Core.h"
#include <map>

struct DOOM_API Mesh {
public:
	std::string name;
	float* verteces = nullptr;
	uint32_t vertecesSize = 0;
	float* vertecesForNormals = nullptr;
	uint32_t vertecesSizeForNormals = 0;
	uint32_t* indicesForNormals = nullptr;
	uint32_t indicesSizeForNormals = 0;
	uint32_t* indices = nullptr;
	uint32_t indicesSize = 0;
	float* normals = nullptr;
	uint32_t normalsSize = 0;
	uint32_t meshSize = 0;
	float* mesh = nullptr;
	uint32_t uvSize = 0;
	float* uv;
	uint32_t uvSizeForVert = 0;
	float* uvForVert;
	uint32_t uvIndexSize = 0;
	uint32_t* uvIndex;
	float* btangent;
	float* tangent;

	Mesh(std::string name) : name(name){}

	~Mesh() {
		delete[] mesh;
	}
};

class DOOM_API MeshManager {
private:
	static std::map<std::string, Mesh*> Meshes;
public:
	static void LoadMesh(std::string name, std::string filepath);

	static Mesh* GetMesh(std::string name);

	static void DeleteMesh(std::string name);

	static void DeleteMesh(Mesh* mesh);

	static void ShutDown();
};

#endif // MESH_H