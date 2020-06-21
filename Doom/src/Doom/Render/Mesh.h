#ifndef MESH_H
#define MESH_H

#include <iostream>
#include "../Core/Core.h"
#include <map>

struct DOOM_API Mesh {
public:
	std::string name;
	double* verteces = nullptr;
	uint32_t vertecesSize = 0;
	double* vertecesForNormals = nullptr;
	uint32_t vertecesSizeForNormals = 0;
	uint32_t* indicesForNormals = nullptr;
	uint32_t indicesSizeForNormals = 0;
	uint32_t* indices = nullptr;
	uint32_t indicesSize = 0;
	double* normals = nullptr;
	uint32_t normalsSize = 0;
	uint32_t meshSize = 0;
	double* mesh = nullptr;

	Mesh(std::string name) : name(name){}

	~Mesh() {
		//std::cout << "Mesh " << name.c_str() << " destroyed\n";
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