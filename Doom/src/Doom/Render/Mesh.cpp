#include "pch.h"
#include "Mesh.h"
#include "../FbxLoader/fbxdocument.h"
#include "../Core/ThreadPool.h"
#include "../Objects/GameObject.h"
#include "../STLloader/STLloader.h"
#include "Instancing.h"
#include "ObjLoader/OBJLoader.h"

using namespace Doom;

void Mesh::Init()
{
	if (m_IsInitialized)
		return;
	m_Va.Init();
	m_Vb.Init(m_VertAttrib, m_VertAttribSize * sizeof(float));
	m_Layout.Push<float>(3); //verteces
	m_Layout.Push<float>(3); //normals
	m_Layout.Push<float>(2); //uvs
	m_Layout.Push<float>(3); //color
	m_Layout.Push<float>(3); //tangent
	m_Layout.Push<float>(3); //btangent
	m_Va.AddBuffer(m_Vb, m_Layout);
	m_Ib.Init(m_Indices, m_IndicesSize);
	m_Ib.UnBind();
	m_Vb.UnBind();
	m_Va.UnBind();

	m_IsInitialized = true;
}

void Doom::Mesh::Refresh()
{
	m_Va.Clear();
	m_Vb.Clear();
	m_Layout.Clear();
	m_Ib.Clear();

	m_IsInitialized = false;
	Init();
}

Mesh::~Mesh()
{
	delete[] m_VertAttrib;
	delete[] m_Indices;
}

int Doom::Mesh::GetAmountOfMeshes()
{
	return s_Meshes.size();
}

void Mesh::LoadMesh(std::string name, std::string filePath, size_t meshId)
{
	if (meshId > 0)
	{
		name = name.append(std::to_string(meshId));
	}
	auto iter = s_Meshes.find(name);
	if (iter != s_Meshes.end()) return;

	std::string subStr = filePath.substr(filePath.size() - 3, 3);
	if (subStr == "fbx")
	{
		fbx::FBXDocument doc;
		s_Meshes.insert(std::make_pair(name, doc.LoadMesh(name, filePath, meshId)));
	}
	else if (subStr == "stl")
	{
		s_Meshes.insert(std::make_pair(name, StlLoader::LoadMesh(name, filePath)));
	}
	else if (subStr == "obj")
	{
		Mesh* mesh = objl::Loader::Load(filePath);
		s_Meshes.insert(std::make_pair(mesh->m_Name, mesh));
	}
	else return;
	std::vector<Renderer3D*> New;
#define	_LOAD_MESH_
	Mesh* mesh = GetMesh(name);
#undef _LOAD_MESH_
	if (mesh == nullptr) return;
	mesh->m_IdOfMeshInFile = meshId;
	mesh->Init();
	Instancing::GetInstance()->m_InstancedObjects.insert(std::make_pair(mesh, New));
	Instancing::GetInstance()->Create(mesh);
	Logger::Success("has been loaded!", "Mesh", name.c_str());
}

void Doom::Mesh::LoadScene(std::string filepath)
{
	fbx::FBXDocument doc;
	doc.LoadScene(filepath);
}

void Doom::Mesh::AsyncLoadMesh(std::string nametemp, std::string filePath, size_t meshId)
{
	Doom::ThreadPool::GetInstance().Enqueue([=] {
		std::lock_guard<std::mutex> lock(s_Mtx);
		std::string name = nametemp;
		if (meshId > 0)
			name = name.append(std::to_string(meshId));

		auto iter = s_Meshes.find(name);
		if (iter != s_Meshes.end()) return;
		std::string subStr = filePath.substr(filePath.size() - 3, 3);
		Mesh* mesh = nullptr;
		if (subStr == "fbx")
		{
			fbx::FBXDocument doc;
			mesh = doc.LoadMesh(name, filePath, meshId);
		}
		else if (subStr == "stl")
			mesh = StlLoader::LoadMesh(name, filePath);
		else if (subStr == "obj")
		{
			mesh = objl::Loader::Load(filePath);
			s_Meshes.insert(std::make_pair(mesh->m_Name, mesh));
		}
		else return;
		if (mesh == nullptr) return;
		{

			s_Meshes.insert(std::make_pair(name, mesh));
			s_NeedToInitMeshes.push_back(mesh);
		}
		Logger::Success("has been loaded!", "Mesh", name.c_str());
		});
}

Mesh* Mesh::GetMesh(std::string name)
{
	auto iter = s_Meshes.find(name);
	if (iter != s_Meshes.end())
		return iter->second;
	else
	{
#ifdef _LOAD_MESH_
		Logger::Warning("has not been found!", "Mesh", name.c_str());
#endif
		return nullptr;
	}
}

const char** Doom::Mesh::GetListOfMeshes()
{
	if (s_NamesOfMeshes != nullptr)
		delete[] s_NamesOfMeshes;
	s_NamesOfMeshes = new const char* [s_Meshes.size()];
	size_t i = 0;
	for (auto mesh = s_Meshes.begin(); mesh != s_Meshes.end(); mesh++)
	{
		s_NamesOfMeshes[i] = mesh->first.c_str();
		i++;
	}
	return s_NamesOfMeshes;
}

void Doom::Mesh::AddMesh(Mesh* mesh)
{
	if (mesh != nullptr)
	{
		s_Meshes.insert(std::make_pair(mesh->m_Name, mesh));
	}
}

void Mesh::GetMeshWhenLoaded(std::string name, void* r)
{
	s_MeshQueue.insert(std::make_pair(name, r));
}

void Mesh::DeleteMesh(std::string name)
{
	auto iter = s_Meshes.find(name);
	if (iter != s_Meshes.end())
	{
		Mesh* mesh = iter->second;
		s_Meshes.erase(iter);
		delete mesh;
	}
}

void Mesh::DeleteMesh(Mesh* mesh)
{
	DeleteMesh(mesh->m_Name);
}

void Mesh::ShutDown()
{
	for (auto i = s_Meshes.begin(); i != s_Meshes.end(); i++)
	{
		Mesh* mesh = i->second;
		delete mesh;
	}
	s_Meshes.clear();
}

void Mesh::DispatchLoadedMeshes()
{
	for (size_t i = 0; i < s_NeedToInitMeshes.size();)
	{
		s_NeedToInitMeshes.back()->Init();
		std::vector<Renderer3D*> New;
		Instancing::GetInstance()->m_InstancedObjects.insert(std::make_pair(s_NeedToInitMeshes[i], New));
		Instancing::GetInstance()->Create(s_NeedToInitMeshes[i]);
		s_NeedToInitMeshes.pop_back();
	}
	if (s_MeshQueue.size() > 0)
	{
		for (auto i = s_MeshQueue.begin(); i != s_MeshQueue.end();)
		{
			auto iter = s_Meshes.find(i->first);
			if (iter != s_Meshes.end())
			{
				Renderer3D* r = static_cast<Renderer3D*>(i->second);
				r->LoadMesh(iter->second);
				r->ChangeRenderTechnic(r->m_RenderTechnic);
				s_MeshQueue.erase(i++);
			}
			else
			{
				i++;
			}
		}
	}
}
