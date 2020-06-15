#include "Render3D.h"
#include "../Objects2D/GameObject.h"

Doom::Renderer3D::Renderer3D(GameObject* _owner)
{
	SetType("Renderer");
	owner = _owner;
	shader = new Shader("src/Shaders/Basic3D.shader");
	tr = owner->GetComponentManager()->GetComponent<Transform>();
	pos = translate(glm::mat4(1.f), glm::vec3(tr->position.x, tr->position.y, 0));
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);
	layout->Push<float> (2);
	layout->Push<float> (2);
	va->AddBuffer(*this->vb, *this->layout);
	va->UnBind();
	shader->UnBind();
	vb->UnBind();
	ib->UnBind();
}

void Doom::Renderer3D::Render()
{
	this->shader->Bind();
	this->pos = translate(glm::mat4(1.f), glm::vec3(tr->position.x, tr->position.y, tr->position.z));
	this->viewXscale = scale * view;
	this->MVP = Window::GetCamera().GetProjectionMatrix() * pos * viewXscale;
	Window::GetCamera().RecalculateViewMatrix();
	this->shader->UploadUnifromMat4("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
	this->shader->SetUniformMat4f("u_MVP", this->MVP);
	this->shader->SetUniform4fv("m_color", color);
	this->shader->Bind();
	va->Bind();
	ib->Bind();
	glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
}
