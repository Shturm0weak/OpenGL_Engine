#include "../pch.h"
#include "SphereCollider.h"

using namespace Doom;

Doom::SphereCollider::~SphereCollider()
{
	auto iter = std::find(spheres.begin(), spheres.end(), this);
	if (iter != spheres.end()) {
		spheres.erase(iter);
	}
}

SphereCollider::SphereCollider() {
	mesh = MeshManager::GetMesh("sphere");
	spheres.push_back(this);
	SetType(ComponentType::SPHERECOLLIDER);
}

void Doom::SphereCollider::Render()
{
	Transform* tr = owner->GetComponent<Transform>();
	glm::vec3 scale = tr->GetScale();
	if (isInBoundingBox)
		radius = scale.y;
	else
		radius = sqrtf(scale.x * scale.x + scale.y * scale.y + scale.z * scale.z);
	this->shader->Bind();
	this->shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
	this->shader->SetUniformMat4f("u_Model", glm::translate(GetOwnerOfComponent()->transform->pos, offset));
	this->shader->SetUniformMat4f("u_Scale", glm::scale(glm::mat4(1.0f), glm::vec3(radius, radius, radius)));
	this->shader->SetUniformMat4f("u_View", GetOwnerOfComponent()->transform->view);
	this->shader->SetUniform4fv("u_Color", color);
	mesh->va->Bind();
	mesh->ib->Bind();
	mesh->vb->Bind();
	Renderer::DrawCalls++;
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, mesh->ib->GetCount(), GL_UNSIGNED_INT, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	shader->UnBind();
	mesh->ib->UnBind();
}

bool Doom::SphereCollider::IntersectSphereToSphere(SphereCollider* sp) {
	if (sp == this)
		return false;
	glm::vec3 pos1 = GetOwnerOfComponent()->GetPosition() + offset;
	glm::vec3 pos2 = sp->GetOwnerOfComponent()->GetPosition() + offset;
	glm::vec3 scale = GetOwnerOfComponent()->GetScale();
	float d = glm::distance(pos1, pos2);
	if (d < radius + sp->radius) {
		EventSystem::GetInstance()->SendEvent(EventType::ONCOLLSION,(Listener*)GetOwnerOfComponent(),(void*)(sp));
		return true;
	}
	return false;
}
