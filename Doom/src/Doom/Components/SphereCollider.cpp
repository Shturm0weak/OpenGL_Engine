#include "../pch.h"
#include "SphereCollider.h"

using namespace Doom;

Doom::SphereCollider::~SphereCollider()
{
	auto iter = std::find(s_Spheres.begin(), s_Spheres.end(), this);
	if (iter != s_Spheres.end()) 
	{
		s_Spheres.erase(iter);
	}
}

void Doom::SphereCollider::Copy(const SphereCollider& rhs)
{
	m_Offset = rhs.m_Offset;
	m_Radius = rhs.m_Radius;
	m_IsInBoundingBox = rhs.m_IsInBoundingBox;
}

void Doom::SphereCollider::operator=(const SphereCollider& rhs)
{
	Copy(rhs);
}

Component* Doom::SphereCollider::Create()
{
	return new SphereCollider();
}

Doom::SphereCollider::SphereCollider(const SphereCollider& rhs)
{
	Copy(rhs);
}

SphereCollider::SphereCollider() {
	m_Mesh = MeshManager::GetInstance().GetMesh("sphere");
	s_Spheres.push_back(this);
	//SetType(ComponentType::SPHERECOLLIDER);
}

void Doom::SphereCollider::Render()
{
	Transform* tr = m_OwnerOfCom->GetComponent<Transform>();
	glm::vec3 scale = tr->GetScale();
	if (m_IsInBoundingBox) m_Radius = scale.y;
	else m_Radius = sqrtf(scale.x * scale.x + scale.y * scale.y + scale.z * scale.z);
	this->m_Shader->Bind();
	this->m_Shader->SetUniformMat4f("u_ViewProjection", Window::GetInstance().GetCamera().GetViewProjectionMatrix());
	this->m_Shader->SetUniformMat4f("u_Model", glm::translate(GetOwnerOfComponent()->m_Transform.m_PosMat4, m_Offset));
	this->m_Shader->SetUniformMat4f("u_Scale", glm::scale(glm::mat4(1.0f), glm::vec3(m_Radius, m_Radius, m_Radius)));
	this->m_Shader->SetUniformMat4f("u_View", GetOwnerOfComponent()->m_Transform.m_ViewMat4);
	this->m_Shader->SetUniform4fv("u_Color", m_Color);
	m_Mesh->m_Va.Bind();
	m_Mesh->m_Ib.Bind();
	m_Mesh->m_Vb.Bind();
	Renderer::s_Stats.m_DrawCalls++;
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, m_Mesh->m_Ib.m_count, GL_UNSIGNED_INT, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	m_Shader->UnBind();
	m_Mesh->m_Ib.UnBind();
}

bool Doom::SphereCollider::IntersectSphereToSphere(SphereCollider* sp) {
	if (sp == this) return false;
	glm::vec3 pos1 = GetOwnerOfComponent()->GetPosition() + m_Offset;
	glm::vec3 pos2 = sp->GetOwnerOfComponent()->GetPosition() + m_Offset;
	glm::vec3 scale = GetOwnerOfComponent()->GetScale();
	float d = glm::distance(pos1, pos2);
	if (d < m_Radius + sp->m_Radius) 
	{
		EventSystem::GetInstance().SendEvent(EventType::ONCOLLISION,(Listener*)GetOwnerOfComponent(),(void*)(sp));
		return true;
	}
	return false;
}