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
	glm::vec3 position = Utils::GetPosition(pos);
	if (prevPosition.x + prevPosition.y + prevPosition.z != position.x + position.y + position.z) {
		sr = owner->GetComponentManager()->GetComponent<SpriteRenderer>();
		if (sr == NULL)
			return;
		prevPosition = position;
		sr->Update(position);
	}
}

glm::vec3 Doom::Transform::GetPosition()
{
	return Utils::GetPosition(pos);
}

glm::vec3 Doom::Transform::GetScale()
{
	return Utils::GetScale(scale);
}

void Transform::Move(float speedX,float speedY,float speedZ) {
	glm::vec3 position = Utils::GetPosition(pos);
	position.x += speedX * DeltaTime::GetDeltaTime();
	position.y += speedY * DeltaTime::GetDeltaTime();
	position.z += speedZ * DeltaTime::GetDeltaTime();
	pos = glm::translate(glm::mat4(1.0f),position);
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr) {
		col->UpdateCollision(position.x, position.y, pos, view, scale);
	}
	RealVertexPositions();
	if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->Move(speedX, speedY, speedZ);
		}
	}
	EventSystem::GetInstance()->SendEvent(EventType::ONMOVE,(Listener*)owner);
}

void Transform::RotateOnce(float x, float y, float z,bool isRad) {
	sr = owner->GetComponentManager()->GetComponent<Irenderer>();
		if (isRad) {
			rotation.x = x;
			rotation.y = y;
			rotation.z = z;
		}
		else {
			rotation.x = (-x * (2 * 3.14159f) / 360.0f);
			rotation.y = (-y * (2 * 3.14159f) / 360.0f);
			rotation.z = (-z * (2 * 3.14159f) / 360.0f);
		}
		view = glm::mat4(1.0f);
		view = glm::rotate(view, rotation.x, glm::vec3(1, 0, 0));
		view = glm::rotate(view, rotation.y, glm::vec3(0, 1, 0));
		view = glm::rotate(view, rotation.z, glm::vec3(0, 0, 1));
		RealVertexPositions();
		if (col == nullptr) {
			col = owner->component_manager->GetComponent<RectangleCollider2D>();
		}
		if (col != nullptr) {
			glm::vec3 position = Utils::GetPosition(pos);
			col->UpdateCollision(position.x, position.y, pos, view, scale);
		}
		EventSystem::GetInstance()->SendEvent(EventType::ONROTATE, (Listener*)owner);
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
	glm::vec3 position = Utils::GetPosition(pos);
	glm::vec3 axis = tempAxis;
	glm::vec3 b = glm::vec3(0, 1, 0);
	float angleRad = acosf((a.x * b.x + a.y * b.y + a.z * b.z) / ((sqrtf(a.x * a.x + a.y * a.y + a.z * a.z) * (sqrtf(b.x * b.x + b.y * b.y + b.z * b.z)))));
	if (ViewPort::GetInstance()->GetMousePositionToWorldSpace().x > position.x)
		angleRad = -angleRad;
	axis *= angleRad;
	view = glm::mat4(1.0f);
	view = glm::rotate(view, angleRad, axis);
	RealVertexPositions();
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr) {
		col->UpdateCollision(position.x, position.y, pos, view, scale);
	}
	EventSystem::GetInstance()->SendEvent(EventType::ONROTATE, (Listener*)owner);
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
	rotation.x = (-x * (2 * 3.14159f) / 360.0f);
	rotation.y = (-y * (2 * 3.14159f) / 360.0f);
	rotation.z = (-z * (2 * 3.14159f) / 360.0f);
	view = glm::rotate(view, rotation.x * DeltaTime::GetDeltaTime(), glm::vec3(1, 0, 0));
	view = glm::rotate(view, rotation.y * DeltaTime::GetDeltaTime(), glm::vec3(0, 1, 0));
	view = glm::rotate(view, rotation.z * DeltaTime::GetDeltaTime(), glm::vec3(0, 0, 1));
	RealVertexPositions();
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr) {
		glm::vec3 position = Utils::GetPosition(pos);
		col->UpdateCollision(position.x, position.y, pos, view, scale);
	}
	EventSystem::GetInstance()->SendEvent(EventType::ONROTATE, (Listener*)owner);
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
	scale = glm::scale(glm::mat4(1.f), vScale);
	RealVertexPositions();
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr){
		glm::vec3 position = Utils::GetPosition(pos);
		col->UpdateCollision(position.x, position.y, pos, view, scale);
	}
	EventSystem::GetInstance()->SendEvent(EventType::ONSCALE, (Listener*)owner);
	unsigned int size = owner->GetChilds().size();
	for (unsigned int i = 0; i < size; i++)
	{
		GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
		if (go->Enable == true)
			go->GetComponentManager()->GetComponent<Transform>()->Scale(scaleX, scaleY, scaleZ);
	}
}

void Transform::Translate(float x, float y,float z)
{
	glm::vec3 position = Utils::GetPosition(pos);
	if (owner->Enable) {
		uint32_t size = owner->GetChilds().size();
		for (uint32_t i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true) {
				glm::vec3 _pos = go->GetPosition() - owner->GetPosition();
				go->GetComponentManager()->GetComponent<Transform>()->Translate(_pos.x + x, _pos.y + y, _pos.z + z);
			}
		}
	}
	position.x = x;
	position.y = y;
	position.z = z;
	pos = translate(glm::mat4(1.f), position);
	RealVertexPositions();
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<RectangleCollider2D>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, pos, view, scale);
	EventSystem::GetInstance()->SendEvent(EventType::ONTRANSLATE, (Listener*)owner);
}
