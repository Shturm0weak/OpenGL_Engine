#include "../pch.h"
#include "CubeCollider.h"

using namespace Doom;

void Doom::CubeCollider3D::Delete()
{
	delete m_Va;
	delete m_Vb;
	delete m_Layout;
	delete m_Ib;
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
	component->m_Layout = new VertexBufferLayout();
	component->m_Vb = new VertexBuffer(component->m_Mesh->m_VertAttrib, 36 * 3 * sizeof(float));
	component->m_Va = new VertexArray();
	component->m_Ib = new IndexBuffer(component->m_Mesh->m_Indices, 36);
	component->m_Layout->Push<float>(3);
	component->m_Va->AddBuffer(*component->m_Vb, *component->m_Layout);
	component->m_Va->UnBind();
	component->m_Shader->UnBind();
	component->m_Vb->UnBind();
	component->m_Ib->UnBind();
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
	Mesh* mesh = new Mesh("CubeCollider", "src/Mesh/Primitives/cube.fbx");
	mesh->m_VertAttrib = vertices;
	mesh->m_Indices = indices;
	MeshManager::GetInstance().AddMesh(mesh);
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
	Transform* tr = m_OwnerOfCom->GetComponent<Transform>();
	this->m_Shader->Bind();
	this->m_Shader->SetUniformMat4f("u_ViewProjection", Window::GetInstance().GetCamera().GetViewProjectionMatrix());
	this->m_Shader->SetUniformMat4f("u_Model", glm::translate(tr->m_PosMat4,m_Offset * tr->GetScale())); //??? Probably I need to offset * glm::vec3 not just glm::vec3.y ???
	this->m_Shader->SetUniformMat4f("u_Scale", glm::scale(glm::mat4(1.0f), (glm::abs(m_MinP) + glm::abs(m_MaxP)) * 0.5f) * tr->m_ScaleMat4);
	this->m_Shader->SetUniformMat4f("u_View", tr->m_ViewMat4);
	this->m_Shader->SetUniform4fv("u_Color", m_Color);
	m_Va->Bind();
	m_Ib->Bind();
	m_Vb->Bind();
	Renderer::s_Stats.m_DrawCalls++;
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, m_Ib->GetCount(), GL_UNSIGNED_INT, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	m_Shader->UnBind();
	m_Ib->UnBind();
}