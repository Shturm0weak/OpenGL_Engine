#include "../pch.h"
#include "Transform.h"
#include "../Core/EventSystem.h"
#include "../Objects/GameObject.h"
#include "../Core/ViewPort.h"
#include "../Core/Timer.h"
#include "../Core/Utils.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "Rays/Ray3D.h"

using namespace Doom;

Doom::Transform::Transform(const Transform& rhs)
{
	Copy(rhs);
}

Transform::Transform(GameObject* owner)
{
	m_OwnerOfCom = owner;
 	m_Type = Utils::GetComponentTypeName<Transform>();
}

//oid Transform::RealVertexPositions()
//
//	glm::vec3 position = Utils::GetPosition(m_PosMat4);
//	if (m_PrevPosition.x + m_PrevPosition.y + m_PrevPosition.z != position.x + position.y + position.z) {
//		sr = m_Owner->m_ComponentManager.GetComponent<SpriteRenderer>();
//		if (sr == NULL || sr == nullptr)
//			return;
//		m_PrevPosition = position;
//		((SpriteRenderer*)sr)->Update(position);
//	}
//

void Doom::Transform::Copy(const Transform& rhs)
{
	m_PosMat4 = rhs.m_PosMat4;
	m_ViewMat4 = rhs.m_ViewMat4;
	m_ScaleMat4 = rhs.m_ScaleMat4;
}

void Doom::Transform::operator=(const Transform& rhs)
{
	Copy(rhs);
}

Component* Doom::Transform::Create()
{
	return nullptr;
}

glm::vec3 Doom::Transform::GetRotation()
{
	return m_Rotation;
}

glm::vec3 Doom::Transform::GetPosition()
{
	return std::move(Utils::GetPosition(m_PosMat4));
}

glm::vec3 Doom::Transform::GetScale()
{
	return std::move(Utils::GetScale(m_ScaleMat4));
}

glm::mat4 Doom::Transform::GetTransform()
{
	return std::move(m_PosMat4  * m_ViewMat4 * m_ScaleMat4);
}

void Transform::Move(float x,float y,float z) 
{
	Move(glm::vec3(x, y, z));
}

void Transform::RotateOnce(float x, float y, float z,bool isRad)
{
	RotateOnce(glm::vec3(x, y, z), isRad);
}


void Transform::Rotate(float x, float y, float z, bool isRad) 
{
	Rotate(glm::vec3(x, y, z), isRad);
}

void Transform::Scale(float x, float y,float z)
{
	Scale(glm::vec3(x, y, z));
}

void Transform::Translate(float x, float y,float z)
{
	Translate(glm::vec3(x, y, z));
}

void Doom::Transform::RotateOnce(glm::vec3 dir, glm::vec3 axis)
{
	glm::vec3 position = Utils::GetPosition(m_PosMat4);
	glm::vec3 b = glm::vec3(0, 1, 0);
	float angleRad = acosf((dir.x * b.x + dir.y * b.y + dir.z * b.z) / ((sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z) * (sqrtf(b.x * b.x + b.y * b.y + b.z * b.z)))));
	if (ViewPort::GetInstance().GetMousePositionToWorldSpace().x > position.x)
		angleRad = -angleRad;
	axis *= angleRad;
	m_ViewMat4 = glm::toMat4(glm::quat(axis));

	m_OwnerOfCom->GetComponent<RectangleCollider2D>()->CalculateRealVerPos();
	//RealVertexPositions();
}

void Transform::Move(glm::vec3 vdir) 
{
	glm::vec3 position = Utils::GetPosition(m_PosMat4);
	position.x += vdir.x * DeltaTime::GetDeltaTime();
	position.y += vdir.y * DeltaTime::GetDeltaTime();
	position.z += vdir.z * DeltaTime::GetDeltaTime();
	m_PosMat4 = glm::translate(glm::mat4(1.0f), position);
	
	m_OwnerOfCom->GetComponent<RectangleCollider2D>()->CalculateRealVerPos();
	//RealVertexPositions();

	if (m_OwnerOfCom->m_Enable) 
	{
		unsigned int size = m_OwnerOfCom->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(m_OwnerOfCom->GetChilds()[i]);
			if (go->m_Enable == true)
				go->m_ComponentManager.GetComponent<Transform>()->Move(vdir);
		}
	}
	//EventSystem::GetInstance().SendEvent(EventType::ONMOVE, (Listener*)m_OwnerOfCom);
}

void Transform::RotateOnce(glm::vec3 vangles, bool isRad) 
{
	if (isRad) m_Rotation = vangles;
	else m_Rotation = glm::radians(vangles);
	m_ViewMat4 = glm::toMat4(glm::quat(m_Rotation));
	
	m_OwnerOfCom->GetComponent<RectangleCollider2D>()->CalculateRealVerPos();
	//RealVertexPositions();
}

void Transform::Rotate(glm::vec3 vangles, bool isRad)
{
	if (isRad) m_Rotation = vangles;
	else m_Rotation = glm::radians(vangles);
	m_ViewMat4 = glm::toMat4(glm::quat(m_Rotation));
	
	m_OwnerOfCom->GetComponent<RectangleCollider2D>()->CalculateRealVerPos();
	//RealVertexPositions();

	EventSystem::GetInstance().SendEvent(EventType::ONROTATE, (Listener*)m_OwnerOfCom);
}

void Transform::Scale(glm::vec3 vscale)
{
	m_ScaleMat4 = glm::scale(glm::mat4(1.f), vscale);
	
	m_OwnerOfCom->GetComponent<RectangleCollider2D>()->CalculateRealVerPos();
	//RealVertexPositions();

	//EventSystem::GetInstance().SendEvent(EventType::ONSCALE, (Listener*)m_OwnerOfCom);
}

void Transform::Translate(glm::vec3 vpos)
{
	glm::vec3 position = Utils::GetPosition(m_PosMat4);
	if (m_OwnerOfCom->m_Enable)
	{
		uint32_t size = m_OwnerOfCom->GetChilds().size();
		for (uint32_t i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(m_OwnerOfCom->GetChilds()[i]);
			if (go->m_Enable == true) {
				glm::vec3 _pos = go->GetPosition() - m_OwnerOfCom->GetPosition();
				go->m_ComponentManager.GetComponent<Transform>()->Translate(_pos + vpos);
			}
		}
	}
	m_PosMat4 = translate(glm::mat4(1.f), vpos);
	
	m_OwnerOfCom->GetComponent<RectangleCollider2D>()->CalculateRealVerPos();
	//RealVertexPositions();

	//EventSystem::GetInstance().SendEvent(EventType::ONTRANSLATE, (Listener*)m_OwnerOfCom);
}