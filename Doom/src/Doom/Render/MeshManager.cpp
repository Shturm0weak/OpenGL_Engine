#include "MeshManager.h"
#include "../FbxLoader/fbxdocument.h"
#include "../Core/ThreadPool.h"
#include "../Objects2D/GameObject.h"
#include "Instancing.h"

using namespace Doom;

void MeshManager::LoadMesh(std::string name, std::string filepath)
{
	auto iter = Meshes.find(name);
	if (iter != Meshes.end())
		return;
	fbx::FBXDocument doc;
	Meshes.insert(std::make_pair(name, doc.LoadMesh(name, filepath)));
	std::vector<Renderer3D*> New;
	Mesh* mesh = GetMesh(name);
	mesh->Init();
	Instancing::Instance()->instancedObjects.insert(std::make_pair(mesh,New));
	Instancing::Instance()->Create(mesh);
}

void Doom::MeshManager::AsyncLoadMesh(std::string name, std::string filepath)
{
	Doom::ThreadPool::Instance()->enqueue([=] {
		auto iter = Meshes.find(name);
		if (iter != Meshes.end())
			return;
		fbx::FBXDocument doc;
		Meshes.insert(std::make_pair(name, doc.LoadMesh(name, filepath)));
		dispatch = true;
		std::vector<Renderer3D*> New;
		Mesh* mesh = GetMesh(name);
		Instancing::Instance()->instancedObjects.insert(std::make_pair(mesh, New));
		Instancing::Instance()->Create(mesh);
		std::cout << BOLDGREEN << "Mesh " << name << " has been loaded\n" << RESET;
	});
}

Mesh * MeshManager::GetMesh(std::string name)
{
	auto iter = Meshes.find(name);
	if (iter != Meshes.end()) {
		return iter->second;
	}
	else {
		std::cout << RED << "No such mesh\n" << RESET;
		return nullptr;
	}
}

void MeshManager::GetMeshWhenLoaded(std::string name, void* r)
{
	auto iter = Meshes.find(name);
	if(iter == Meshes.end())
		meshQueue.insert(std::make_pair(name,r));
	else {
		iter->second->Init();
		static_cast<Doom::Renderer3D*>(r)->LoadMesh(iter->second);
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

void MeshManager::DispatchLoadedMeshes()
{
	if (dispatch) {
		for (auto i = meshQueue.begin(); i != meshQueue.end(); i++)
		{
			auto iter = Meshes.find(i->first);
			if (iter != Meshes.end()) {
				iter->second->Init();
				static_cast<Doom::Renderer3D*>(i->second)->LoadMesh(iter->second);
				meshQueue.erase(i);
				//std::cout << meshQueue.size() << std::endl;
				return;
			}
		}
		dispatch = false;
	}
}
