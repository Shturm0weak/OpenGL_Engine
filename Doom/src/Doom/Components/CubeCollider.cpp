#include "../pch.h"
#include "CubeCollider.h"

using namespace Doom;

void Doom::CubeCollider3D::Delete()
{
	s_FreeMemory.push_back(m_MemoryPoolPtr);
	auto iter = std::find(s_Colliders.begin(), s_Colliders.end(), this);
	if (iter != s_Colliders.end())
		s_Colliders.erase(iter);
}

Component* Doom::CubeCollider3D::Create()
{
	char* ptr = Utils::PreAllocateMemory<CubeCollider3D>(s_MemoryPool, s_FreeMemory);
	CubeCollider3D* component = (CubeCollider3D*)((void*)ptr);// = new(iter->first + iter->second * sizeof(CubeCollider3D)) CubeCollider3D();
	component->m_MemoryPoolPtr = ptr;
	component->m_Mesh = MeshManager::GetInstance().GetMesh("CubeCollider");
	s_Colliders.push_back(component);
	return component;
}

void Doom::CubeCollider3D::InitMesh()
{
	float* vertices = new float[36 * 3]{
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
	};
	uint32_t* indices = new uint32_t[36];
	for (size_t i = 0; i < 36; i++)
	{
		indices[i] = i;
	}
	if (MeshManager::GetInstance().GetMesh("CubeCollider") == nullptr || MeshManager::GetInstance().GetMesh("CubeCollider")->m_IsInitialized == false)
	{
		Mesh* mesh = new Mesh("CubeCollider", "src/Mesh/Primitives/cube.fbx");
		mesh->m_VertAttrib = vertices;
		mesh->m_Indices = indices;
		mesh->m_Va.Init();
		mesh->m_Vb.Init(mesh->m_VertAttrib, 36 * 3 * sizeof(float));
		mesh->m_Ib.Init(mesh->m_Indices, 36);
		mesh->m_Layout.Push<float>(3);
		mesh->m_Va.AddBuffer(mesh->m_Vb, mesh->m_Layout);
		mesh->m_Ib.UnBind();
		mesh->m_Vb.UnBind();
		mesh->m_Va.UnBind();
		mesh->m_IsInitialized = true;
		MeshManager::GetInstance().AddMesh(mesh);
	}
}

Doom::CubeCollider3D::~CubeCollider3D()
{
	
}

CubeCollider3D::CubeCollider3D() 
{
	//SetType(ComponentType::CUBECOLLIDER3D);
}

void Doom::CubeCollider3D::Render()
{
	Transform& tr = m_OwnerOfCom->m_Transform;
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("u_ViewProjection", Window::GetInstance().GetCamera().GetViewProjectionMatrix());
	m_Shader->SetUniformMat4f("u_Model", glm::translate(tr.m_PosMat4,m_Offset * tr.GetScale())); //??? Probably I need to offset * glm::vec3 not just glm::vec3.y ???
	m_Shader->SetUniformMat4f("u_Scale", glm::scale(glm::mat4(1.0f), ((m_MaxP) - (m_MinP)) * 0.5f) * tr.m_ScaleMat4);
	m_Shader->SetUniformMat4f("u_View", tr.m_ViewMat4);
	m_Shader->SetUniform4fv("u_Color", m_Color);
	m_Mesh->m_Va.Bind();
	m_Mesh->m_Ib.Bind();
	Renderer::s_Stats.m_DrawCalls++;
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, m_Mesh->m_Ib.m_count, GL_UNSIGNED_INT, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	m_Shader->UnBind();
	m_Mesh->m_Ib.UnBind();
}