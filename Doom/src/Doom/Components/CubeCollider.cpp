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
	delete vb;
	delete ib;
	delete va;
	auto iter = std::find(colliders.begin(), colliders.end(), this);
	if (iter != colliders.end()) {
		colliders.erase(iter);
	}
}

CubeCollider3D::CubeCollider3D() {
	mesh = MeshManager::GetMesh("CubeCollider");
	layout = new VertexBufferLayout();
	vb = new VertexBuffer(mesh->mesh, 36 * 3 * sizeof(float));
	va = new VertexArray();
	ib = new IndexBuffer(mesh->indices, 36);
	layout->Push<float>(3);
	va->AddBuffer(*this->vb, *this->layout);
	va->UnBind();
	shader->UnBind();
	vb->UnBind();
	ib->UnBind();
	colliders.push_back(this);
	//SetType(ComponentType::CUBECOLLIDER3D);
}

void Doom::CubeCollider3D::Render()
{
	Transform* tr = owner->GetComponent<Transform>();
	this->shader->Bind();
	this->shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
	this->shader->SetUniformMat4f("u_Model", glm::translate(tr->pos,offset * tr->GetScale())); //??? Probably I need to offset * glm::vec3 not just glm::vec3.y ???
	this->shader->SetUniformMat4f("u_Scale", glm::scale(glm::mat4(1.0f), (glm::abs(minP) + glm::abs(maxP)) * 0.5f) * tr->scale);
	this->shader->SetUniformMat4f("u_View", tr->view);
	this->shader->SetUniform4fv("u_Color", color);
	va->Bind();
	ib->Bind();
	vb->Bind();
	Renderer::DrawCalls++;
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	shader->UnBind();
	ib->UnBind();
}