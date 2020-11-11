#include "MeshManager.h"
#include "../FbxLoader/fbxdocument.h"
#include "../Core/ThreadPool.h"
#include "../Objects2D/GameObject.h"
#include "../STLloader/STLloader.h"
#include "Instancing.h"

using namespace Doom;

void MeshManager::LoadMesh(std::string name, std::string filepath)
{
	auto iter = Meshes.find(name);
	if (iter != Meshes.end())
		return;
	std::string subStr = filepath.substr(filepath.size() - 3, 3);
	if (subStr == "fbx") {
		fbx::FBXDocument doc;
		Meshes.insert(std::make_pair(name, doc.LoadMesh(name, filepath)));
	}
	else if (subStr == "stl") {
		Meshes.insert(std::make_pair(name, StlLoader::LoadMesh(name, filepath)));
	}
	else {
		return;
	}
	std::vector<Renderer3D*> New;
	#define	_LOAD_MESH_
	Mesh* mesh = GetMesh(name);
	#undef _LOAD_MESH_
	mesh->Init();
	Instancing::Instance()->instancedObjects.insert(std::make_pair(mesh,New));
	Instancing::Instance()->Create(mesh);
	std::cout << BOLDGREEN << "Mesh: <" << NAMECOLOR << name << BOLDGREEN  << "> has been loaded\n" << RESET;
}

void Doom::MeshManager::AsyncLoadMesh(std::string name, std::string filepath)
{
	Doom::ThreadPool::Instance()->enqueue([=] {
		auto iter = Meshes.find(name);
		if (iter != Meshes.end())
			return;
		std::string subStr = filepath.substr(filepath.size() - 3, 3);
		if (subStr == "fbx") {
			fbx::FBXDocument doc;
			Meshes.insert(std::make_pair(name, doc.LoadMesh(name, filepath)));
		}
		else if (subStr == "stl") {
			Meshes.insert(std::make_pair(name, StlLoader::LoadMesh(name, filepath)));
		}
		else {
			return;
		}
		Mesh* mesh = GetMesh(name);
		needToInitMeshes.push_back(mesh);
		std::cout << BOLDGREEN << "Mesh: <" << NAMECOLOR << name << BOLDGREEN << "> has been loaded\n" << RESET;
	});
}

Mesh * MeshManager::GetMesh(std::string name)
{
	auto iter = Meshes.find(name);
	if (iter != Meshes.end()) {
		return iter->second;
	}
	else {
#ifdef _LOAD_MESH_
		std::cout << RED << "Mesh: <" NAMECOLOR << name << RED << "> has not been found!\n" << RESET;
#endif
		return nullptr;
	}
}

const char ** Doom::MeshManager::GetListOfMeshes()
{
	if (listOfMeshes != nullptr)
		delete[] listOfMeshes;
	listOfMeshes = new const char*[Meshes.size()];
	uint32_t i = 0;
	for (auto mesh = Meshes.begin(); mesh != Meshes.end(); mesh++) {
		listOfMeshes[i] = mesh->first.c_str();
		i++;
	}
	return listOfMeshes;
}

void Doom::MeshManager::AddMesh(Mesh * mesh)
{
	if (mesh != nullptr) {
		Meshes.insert(std::make_pair(mesh->name, mesh));
	}
}

void MeshManager::GetMeshWhenLoaded(std::string name, void* r)
{
	//auto iter = Meshes.find(name);
	//if (iter == Meshes.end()) {
		meshQueue.insert(std::make_pair(name, r));
	//}
	//else {
	//	Renderer3D* render = static_cast<Renderer3D*>(r);
	//	render->mesh = GetMesh(name);
	//}
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

void MeshManager::DispatchLoadedMeshes()
{
	uint32_t size = needToInitMeshes.size();
	if (size > 0) {
		for (uint32_t i = 0; i < size; i++)
		{
			needToInitMeshes[i]->Init();
			std::vector<Renderer3D*> New;
			Instancing::Instance()->instancedObjects.insert(std::make_pair(needToInitMeshes[i], New));
			Instancing::Instance()->Create(needToInitMeshes[i]);
		}
		needToInitMeshes.clear();
	}
	if (meshQueue.size() > 0) {
		for (auto i = meshQueue.begin(); i != meshQueue.end();)
		{
			auto iter = Meshes.find(i->first);
			if (iter != Meshes.end()) {
				Renderer3D* r = static_cast<Renderer3D*>(i->second);
				r->LoadMesh(iter->second);
				meshQueue.erase(i++);
			}
			else
				i++;
		}
	}
}
