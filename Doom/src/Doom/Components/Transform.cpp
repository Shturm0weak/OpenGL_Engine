#include "../pch.h"
#include "Transform.h"
#include "../Core/EventSystem.h"
#include "../Objects/GameObject.h"
#include "../Core/ViewPort.h"
#include "../Core/Timer.h"
#include "../Core/Utils.h"

using namespace Doom;

Transform::Transform() {
	//SetType(ComponentType::TRANSFORM);
}

void Transform::RealVertexPositions()
{
	glm::vec3 position = Utils::GetPosition(m_PosMat4);
	if (m_PrevPosition.x + m_PrevPosition.y + m_PrevPosition.z != position.x + position.y + position.z) {
		sr = m_Owner->GetComponentManager()->GetComponent<SpriteRenderer>();
		if (sr == NULL)
			return;
		m_PrevPosition = position;
		sr->Update(position);
	}
}

glm::vec3 Doom::Transform::GetRotation()
{
	return m_Rotation;
}

glm::vec3 Doom::Transform::GetPosition()
{
	return Utils::GetPosition(m_PosMat4);
}

glm::vec3 Doom::Transform::GetScale()
{
	return Utils::GetScale(m_ScaleMat4);
}

void Transform::Move(float speedX,float speedY,float speedZ) {
	glm::vec3 position = Utils::GetPosition(m_PosMat4);
	position.x += speedX * DeltaTime::GetDeltaTime();
	position.y += speedY * DeltaTime::GetDeltaTime();
	position.z += speedZ * DeltaTime::GetDeltaTime();
	m_PosMat4 = glm::translate(glm::mat4(1.0f),position);
	if (col == nullptr) {
		col = m_Owner->m_ComponentManager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr) {
		col->UpdateCollision(position.x, position.y, m_PosMat4, m_ViewMat4, m_ScaleMat4);
	}
	RealVertexPositions();
	if (m_Owner->m_Enable) {
		unsigned int size = m_Owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(m_Owner->GetChilds()[i]);
			if (go->m_Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->Move(speedX, speedY, speedZ);
		}
	}
	EventSystem::GetInstance()->SendEvent(EventType::ONMOVE,(Listener*)m_Owner);
}

void Transform::RotateOnce(float x, float y, float z,bool isRad) {
	sr = m_Owner->GetComponentManager()->GetComponent<Irenderer>();
		if (isRad) {
			m_Rotation.x = x;
			m_Rotation.y = y;
			m_Rotation.z = z;
		}
		else {
			m_Rotation.x = (-x * (2 * 3.14159f) / 360.0f);
			m_Rotation.y = (-y * (2 * 3.14159f) / 360.0f);
			m_Rotation.z = (-z * (2 * 3.14159f) / 360.0f);
		}
		m_ViewMat4 = glm::mat4(1.0f);
		m_ViewMat4 = glm::rotate(m_ViewMat4, m_Rotation.x, glm::vec3(1, 0, 0));
		m_ViewMat4 = glm::rotate(m_ViewMat4, m_Rotation.y, glm::vec3(0, 1, 0));
		m_ViewMat4 = glm::rotate(m_ViewMat4, m_Rotation.z, glm::vec3(0, 0, 1));
		RealVertexPositions();
		if (col == nullptr) {
			col = m_Owner->m_ComponentManager->GetComponent<RectangleCollider2D>();
		}
		if (col != nullptr) {
			glm::vec3 position = Utils::GetPosition(m_PosMat4);
			col->UpdateCollision(position.x, position.y, m_PosMat4, m_ViewMat4, m_ScaleMat4);
		}
		EventSystem::GetInstance()->SendEvent(EventType::ONROTATE, (Listener*)m_Owner);
		/*if (owner->Enable) {
			unsigned int size = owner->GetChilds().size();
			for (unsigned int i = 0; i < size; i++)
			{
				GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
				if (go->Enable == true)
					go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(rotation.x, rotation.y, rotation.z);
			}
		}*/
}

void Doom::Transform::RotateOnce(glm::vec3 a, glm::vec3 tempAxis)
{
	glm::vec3 position = Utils::GetPosition(m_PosMat4);
	glm::vec3 axis = tempAxis;
	glm::vec3 b = glm::vec3(0, 1, 0);
	float angleRad = acosf((a.x * b.x + a.y * b.y + a.z * b.z) / ((sqrtf(a.x * a.x + a.y * a.y + a.z * a.z) * (sqrtf(b.x * b.x + b.y * b.y + b.z * b.z)))));
	if (ViewPort::GetInstance()->GetMousePositionToWorldSpace().x > position.x)
		angleRad = -angleRad;
	axis *= angleRad;
	m_ViewMat4 = glm::mat4(1.0f);
	m_ViewMat4 = glm::rotate(m_ViewMat4, angleRad, axis);
	RealVertexPositions();
	if (col == nullptr) {
		col = m_Owner->m_ComponentManager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr) {
		col->UpdateCollision(position.x, position.y, m_PosMat4, m_ViewMat4, m_ScaleMat4);
	}
	EventSystem::GetInstance()->SendEvent(EventType::ONROTATE, (Listener*)m_Owner);
	/*if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(axis.x, axis.y, axis.z);
		}
	}*/
}

void Transform::Rotate(float x, float y, float z) {
	m_Rotation.x = (-x * (2 * 3.14159f) / 360.0f);
	m_Rotation.y = (-y * (2 * 3.14159f) / 360.0f);
	m_Rotation.z = (-z * (2 * 3.14159f) / 360.0f);
	m_ViewMat4 = glm::rotate(m_ViewMat4, m_Rotation.x * DeltaTime::GetDeltaTime(), glm::vec3(1, 0, 0));
	m_ViewMat4 = glm::rotate(m_ViewMat4, m_Rotation.y * DeltaTime::GetDeltaTime(), glm::vec3(0, 1, 0));
	m_ViewMat4 = glm::rotate(m_ViewMat4, m_Rotation.z * DeltaTime::GetDeltaTime(), glm::vec3(0, 0, 1));
	RealVertexPositions();
	if (col == nullptr) {
		col = m_Owner->m_ComponentManager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr) {
		glm::vec3 position = Utils::GetPosition(m_PosMat4);
		col->UpdateCollision(position.x, position.y, m_PosMat4, m_ViewMat4, m_ScaleMat4);
	}
	EventSystem::GetInstance()->SendEvent(EventType::ONROTATE, (Listener*)m_Owner);
	/*if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(rotation.x, rotation.y, rotation.z);
		}
	}*/
}

void Transform::Scale(float scaleX, float scaleY,float scaleZ) {
	glm::vec3 vScale(scaleX, scaleY, scaleZ);
	m_ScaleMat4 = glm::scale(glm::mat4(1.f), vScale);
	RealVertexPositions();
	if (col == nullptr) {
		col = m_Owner->m_ComponentManager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr){
		glm::vec3 position = Utils::GetPosition(m_PosMat4);
		col->UpdateCollision(position.x, position.y, m_PosMat4, m_ViewMat4, m_ScaleMat4);
	}
	EventSystem::GetInstance()->SendEvent(EventType::ONSCALE, (Listener*)m_Owner);
	unsigned int size = m_Owner->GetChilds().size();
	for (unsigned int i = 0; i < size; i++)
	{
		GameObject* go = static_cast<GameObject*>(m_Owner->GetChilds()[i]);
		if (go->m_Enable == true)
			go->GetComponentManager()->GetComponent<Transform>()->Scale(scaleX, scaleY, scaleZ);
	}
}

void Transform::Translate(float x, float y,float z)
{
	glm::vec3 position = Utils::GetPosition(m_PosMat4);
	if (m_Owner->m_Enable) {
		uint32_t size = m_Owner->GetChilds().size();
		for (uint32_t i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(m_Owner->GetChilds()[i]);
			if (go->m_Enable == true) {
				glm::vec3 _pos = go->GetPosition() - m_Owner->GetPosition();
				go->GetComponentManager()->GetComponent<Transform>()->Translate(_pos.x + x, _pos.y + y, _pos.z + z);
			}
		}
	}
	position.x = x;
	position.y = y;
	position.z = z;
	m_PosMat4 = translate(glm::mat4(1.f), position);
	RealVertexPositions();
	if (col == nullptr) {
		col = m_Owner->m_ComponentManager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, m_PosMat4, m_ViewMat4, m_ScaleMat4);
	EventSystem::GetInstance()->SendEvent(EventType::ONTRANSLATE, (Listener*)m_Owner);
}

void Transform::Move(glm::vec3 vdir) {
	glm::vec3 position = Utils::GetPosition(m_PosMat4);
	position.x += vdir.x * DeltaTime::GetDeltaTime();
	position.y += vdir.y * DeltaTime::GetDeltaTime();
	position.z += vdir.z * DeltaTime::GetDeltaTime();
	m_PosMat4 = glm::translate(glm::mat4(1.0f), position);
	if (col == nullptr) {
		col = m_Owner->m_ComponentManager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr) {
		col->UpdateCollision(position.x, position.y, m_PosMat4, m_ViewMat4, m_ScaleMat4);
	}
	RealVertexPositions();
	if (m_Owner->m_Enable) {
		unsigned int size = m_Owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(m_Owner->GetChilds()[i]);
			if (go->m_Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->Move(vdir);
		}
	}
	EventSystem::GetInstance()->SendEvent(EventType::ONMOVE, (Listener*)m_Owner);
}

void Transform::RotateOnce(glm::vec3 vangles, bool isRad) {
	sr = m_Owner->GetComponentManager()->GetComponent<Irenderer>();
	if (isRad) {
		m_Rotation.x = vangles.x;
		m_Rotation.y = vangles.y;
		m_Rotation.z = vangles.z;
	}
	else {
		m_Rotation.x = (-vangles.x * (2 * 3.14159f) / 360.0f);
		m_Rotation.y = (-vangles.y * (2 * 3.14159f) / 360.0f);
		m_Rotation.z = (-vangles.z * (2 * 3.14159f) / 360.0f);
	}
	m_ViewMat4 = glm::mat4(1.0f);
	m_ViewMat4 = glm::rotate(m_ViewMat4, m_Rotation.x, glm::vec3(1, 0, 0));
	m_ViewMat4 = glm::rotate(m_ViewMat4, m_Rotation.y, glm::vec3(0, 1, 0));
	m_ViewMat4 = glm::rotate(m_ViewMat4, m_Rotation.z, glm::vec3(0, 0, 1));
	RealVertexPositions();
	if (col == nullptr) {
		col = m_Owner->m_ComponentManager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr) {
		glm::vec3 position = Utils::GetPosition(m_PosMat4);
		col->UpdateCollision(position.x, position.y, m_PosMat4, m_ViewMat4, m_ScaleMat4);
	}
	EventSystem::GetInstance()->SendEvent(EventType::ONROTATE, (Listener*)m_Owner);
	/*if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(rotation.x, rotation.y, rotation.z);
		}
	}*/
}

void Transform::Rotate(glm::vec3 vangles) {
	m_Rotation.x = (-vangles.x * (2 * 3.14159f) / 360.0f);
	m_Rotation.y = (-vangles.y * (2 * 3.14159f) / 360.0f);
	m_Rotation.z = (-vangles.z * (2 * 3.14159f) / 360.0f);
	m_ViewMat4 = glm::rotate(m_ViewMat4, m_Rotation.x * DeltaTime::GetDeltaTime(), glm::vec3(1, 0, 0));
	m_ViewMat4 = glm::rotate(m_ViewMat4, m_Rotation.y * DeltaTime::GetDeltaTime(), glm::vec3(0, 1, 0));
	m_ViewMat4 = glm::rotate(m_ViewMat4, m_Rotation.z * DeltaTime::GetDeltaTime(), glm::vec3(0, 0, 1));
	RealVertexPositions();
	if (col == nullptr) {
		col = m_Owner->m_ComponentManager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr) {
		glm::vec3 position = Utils::GetPosition(m_PosMat4);
		col->UpdateCollision(position.x, position.y, m_PosMat4, m_ViewMat4, m_ScaleMat4);
	}
	EventSystem::GetInstance()->SendEvent(EventType::ONROTATE, (Listener*)m_Owner);
	/*if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(rotation.x, rotation.y, rotation.z);
		}
	}*/
}

void Transform::Scale(glm::vec3 vscale) {
	m_ScaleMat4 = glm::scale(glm::mat4(1.f), vscale);
	RealVertexPositions();
	if (col == nullptr) {
		col = m_Owner->m_ComponentManager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr) {
		glm::vec3 position = Utils::GetPosition(m_PosMat4);
		col->UpdateCollision(position.x, position.y, m_PosMat4, m_ViewMat4, m_ScaleMat4);
	}
	EventSystem::GetInstance()->SendEvent(EventType::ONSCALE, (Listener*)m_Owner);
	unsigned int size = m_Owner->GetChilds().size();
	for (unsigned int i = 0; i < size; i++)
	{
		GameObject* go = static_cast<GameObject*>(m_Owner->GetChilds()[i]);
		if (go->m_Enable == true)
			go->GetComponentManager()->GetComponent<Transform>()->Scale(vscale);
	}
}

void Transform::Translate(glm::vec3 vpos)
{
	glm::vec3 position = Utils::GetPosition(m_PosMat4);
	if (m_Owner->m_Enable) {
		uint32_t size = m_Owner->GetChilds().size();
		for (uint32_t i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(m_Owner->GetChilds()[i]);
			if (go->m_Enable == true) {
				glm::vec3 _pos = go->GetPosition() - m_Owner->GetPosition();
				go->GetComponentManager()->GetComponent<Transform>()->Translate(_pos + vpos);
			}
		}
	}
	m_PosMat4 = translate(glm::mat4(1.f), vpos);
	RealVertexPositions();
	if (col == nullptr) {
		col = m_Owner->m_ComponentManager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, m_PosMat4, m_ViewMat4, m_ScaleMat4);
	EventSystem::GetInstance()->SendEvent(EventType::ONTRANSLATE, (Listener*)m_Owner);
}
