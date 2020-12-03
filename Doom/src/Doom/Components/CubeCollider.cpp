#include "../pch.h"
#include "CubeCollider.h"

using namespace Doom;

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
		indices[i] = i;
	Mesh* mesh = new Mesh("CubeCollider","src/Mesh/Primitives/cube.fbx");
	mesh->mesh = vertices;
	mesh->indices = indices;
	MeshManager::AddMesh(mesh);
}

Doom::CubeCollider3D::~CubeCollider3D()
{
	delete m_Vb;
	delete m_Ib;
	delete m_Va;
	auto iter = std::find(colliders.begin(), colliders.end(), this);
	if (iter != colliders.end()) {
		colliders.erase(iter);
	}
}

CubeCollider3D::CubeCollider3D() {
	m_Mesh = MeshManager::GetMesh("CubeCollider");
	m_Layout = new VertexBufferLayout();
	m_Vb = new VertexBuffer(m_Mesh->mesh, 36 * 3 * sizeof(float));
	m_Va = new VertexArray();
	m_Ib = new IndexBuffer(m_Mesh->indices, 36);
	m_Layout->Push<float>(3);
	m_Va->AddBuffer(*this->m_Vb, *this->m_Layout);
	m_Va->UnBind();
	m_Shader->UnBind();
	m_Vb->UnBind();
	m_Ib->UnBind();
	colliders.push_back(this);
	//SetType(ComponentType::CUBECOLLIDER3D);
}

void Doom::CubeCollider3D::Render()
{
	Transform* tr = m_Owner->GetComponent<Transform>();
	this->m_Shader->Bind();
	this->m_Shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
	this->m_Shader->SetUniformMat4f("u_Model", glm::translate(tr->m_PosMat4,m_Offset * tr->GetScale())); //??? Probably I need to offset * glm::vec3 not just glm::vec3.y ???
	this->m_Shader->SetUniformMat4f("u_Scale", glm::scale(glm::mat4(1.0f), (glm::abs(m_MinP) + glm::abs(m_MaxP)) * 0.5f) * tr->m_ScaleMat4);
	this->m_Shader->SetUniformMat4f("u_View", tr->m_ViewMat4);
	this->m_Shader->SetUniform4fv("u_Color", m_Color);
	m_Va->Bind();
	m_Ib->Bind();
	m_Vb->Bind();
	Renderer::DrawCalls++;
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, m_Ib->GetCount(), GL_UNSIGNED_INT, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	m_Shader->UnBind();
	m_Ib->UnBind();
}