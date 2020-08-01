#include "../pch.h"
#include "CubeCollider.h"

using namespace Doom;

CubeCollider3D::CubeCollider3D() {
	layout = new VertexBufferLayout();
	vb = new VertexBuffer(vertices, 36 * 3 * sizeof(float));
	va = new VertexArray();
	for (size_t i = 0; i < 36; i++)
		indeces[i] = i;
	ib = new IndexBuffer(indeces, 36);
	layout->Push<float>(3);
	va->AddBuffer(*this->vb, *this->layout);
	va->UnBind();
	shader->UnBind();
	vb->UnBind();
	ib->UnBind();
	colliders.push_back(this);
	SetType(ComponentTypes::CUBECOLLIDER3D);
}

void Doom::CubeCollider3D::Render()
{
	Renderer3D* r = owner->GetComponent<Renderer3D>();
	this->shader->Bind();
	this->shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
	this->shader->SetUniformMat4f("u_Model", glm::translate(r->pos,offset));
	this->shader->SetUniformMat4f("u_Scale", glm::scale(r->scale,scale));
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