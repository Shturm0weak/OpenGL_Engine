#include "Mesh.h"
#include "../FbxLoader/fbxdocument.h"

void MeshManager::LoadMesh(std::string name, std::string filepath)
{
	fbx::FBXDocument doc;
	Meshes.insert(std::make_pair(name, doc.LoadMesh(&name, filepath)));
}

Mesh * MeshManager::GetMesh(std::string name)
{
	auto iter = Meshes.find(name);
	if (iter != Meshes.end()) {
		return iter->second;
	}
	else {
		std::cout << "No such mesh\n";
		return nullptr;
	}
}

void MeshManager::DeleteMesh(std::string name)
{
	auto iter = Meshes.find(name);
	if (iter != Meshes.end()) {
		Mesh* mesh = iter->second;
		Meshes.erase(iter);
		delete mesh;
	}
}

void MeshManager::DeleteMesh(Mesh * mesh)
{
	DeleteMesh(mesh->name);
}

void MeshManager::ShutDown()
{
	for (auto i = Meshes.begin(); i != Meshes.end(); i++)
	{
		Mesh* mesh = i->second;
		delete mesh;
	}
	Meshes.clear();
}
