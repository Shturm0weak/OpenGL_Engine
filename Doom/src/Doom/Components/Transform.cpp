#include "../pch.h"
#include "Transform.h"
#include "../Core/EventSystem.h"
#include "../Objects2D/GameObject.h"
#include "../Render/ViewPort.h"

using namespace Doom;

Transform::Transform() {
	SetType("Transform");
}

void Transform::init() {
	position.x = owner->position.x;
	position.y = owner->position.y;
	position.z = 0;
}

void Transform::RealVertexPositions()
{
	glm::mat4 scaleXview = owner->view * owner->scale;
	float* pSource;
	pSource = (float*)glm::value_ptr(scaleXview);
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			WorldVerPos[i * 4 + j] = 0;
			for (unsigned int k = 0; k < 4; k++) {
				WorldVerPos[i * 4 + j] += owner->mesh2D[i * 4 + k] * pSource[k * 4 + j];
			}
		}
	}
	pSource = nullptr;
}

void Transform::Move(float speedX,float speedY,float speedZ) {
	position.x += speedX * DeltaTime::GetDeltaTime();
	position.y += speedY * DeltaTime::GetDeltaTime();
	position.z += speedZ * DeltaTime::GetDeltaTime();
	owner->pos = translate(glm::mat4(1.f), glm::vec3(position.x, position.y, position.z));
	RealVertexPositions();
	{
		owner->WorldVertexPositions[0] = WorldVerPos[0];
		owner->WorldVertexPositions[1] = WorldVerPos[1];
		owner->WorldVertexPositions[2] = WorldVerPos[4];
		owner->WorldVertexPositions[3] = WorldVerPos[5];
		owner->WorldVertexPositions[4] = WorldVerPos[8];
		owner->WorldVertexPositions[5] = WorldVerPos[9];
		owner->WorldVertexPositions[6] = WorldVerPos[12];
		owner->WorldVertexPositions[7] = WorldVerPos[13];
	}
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr) {
		col->UpdateCollision(position.x, position.y, owner->pos, owner->view, owner->scale);
	}
	owner->position.x = position.x;
	owner->position.y = position.y;
	if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->Move(speedX, speedY, speedZ);
		}
	}
	//EventSystem::Instance()->SendEvent("OnMove",(Listener*)owner);
}

void Transform::RotateOnce(float theta, glm::vec3 axis) {
	this->angle = (-theta * (2 * 3.14159f) / 360.0f);
	owner->view = glm::mat4(1.0f);
	owner->view = glm::rotate(owner->view, angle, axis);
	RealVertexPositions();
	{
		owner->WorldVertexPositions[0] = WorldVerPos[0];
		owner->WorldVertexPositions[1] = WorldVerPos[1];
		owner->WorldVertexPositions[2] = WorldVerPos[4];
		owner->WorldVertexPositions[3] = WorldVerPos[5];
		owner->WorldVertexPositions[4] = WorldVerPos[8];
		owner->WorldVertexPositions[5] = WorldVerPos[9];
		owner->WorldVertexPositions[6] = WorldVerPos[12];
		owner->WorldVertexPositions[7] = WorldVerPos[13];
	}
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, owner->pos, owner->view, owner->scale);
	//EventSystem::Instance()->SendEvent("OnRotate", (Listener*)owner);
	if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(this->angle, axis);
		}
	}
}

void Doom::Transform::RotateOnce(glm::vec3 a, glm::vec3 axis)
{
	glm::vec3 b = glm::vec3(0,1,0);
	this->angle = acosf((a.x * b.x + a.y * b.y + a.z * b.z)/((sqrtf(a.x * a.x + a.y * a.y + a.z * a.z) * (sqrtf(b.x * b.x + b.y * b.y + b.z * b.z)))));
	if (ViewPort::Instance()->GetMousePositionToWorldSpace().x > position.x)
		this->angle = -this->angle;
	owner->view = glm::mat4(1.0f);
	owner->view = glm::rotate(owner->view, angle, axis);
	RealVertexPositions();
	{
		owner->WorldVertexPositions[0] = WorldVerPos[0];
		owner->WorldVertexPositions[1] = WorldVerPos[1];
		owner->WorldVertexPositions[2] = WorldVerPos[4];
		owner->WorldVertexPositions[3] = WorldVerPos[5];
		owner->WorldVertexPositions[4] = WorldVerPos[8];
		owner->WorldVertexPositions[5] = WorldVerPos[9];
		owner->WorldVertexPositions[6] = WorldVerPos[12];
		owner->WorldVertexPositions[7] = WorldVerPos[13];
	}
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, owner->pos, owner->view, owner->scale);
	//EventSystem::Instance()->SendEvent("OnRotate", (Listener*)owner);
	if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(angle, axis);
		}
	}
}

void Transform::Rotate(float theta, glm::vec3 axis) {
	this->angle = (-theta * (2 * 3.14159f) / 360.0f);
	owner->view = glm::rotate(owner->view, angle * DeltaTime::GetDeltaTime(), axis);
	RealVertexPositions();
	{
		owner->WorldVertexPositions[0] = WorldVerPos[0];
		owner->WorldVertexPositions[1] = WorldVerPos[1];
		owner->WorldVertexPositions[2] = WorldVerPos[4];
		owner->WorldVertexPositions[3] = WorldVerPos[5];
		owner->WorldVertexPositions[4] = WorldVerPos[8];
		owner->WorldVertexPositions[5] = WorldVerPos[9];
		owner->WorldVertexPositions[6] = WorldVerPos[12];
		owner->WorldVertexPositions[7] = WorldVerPos[13];
	}
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, owner->pos, owner->view, owner->scale);
	//EventSystem::Instance()->SendEvent("OnRotate",(Listener*)owner);
	if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(this->angle, axis);
		}
	}
}

void Transform::Scale(float scaleX, float scaleY,float scaleZ) {
	owner->scale = glm::scale(glm::mat4(1.f), glm::vec3(scaleX, scaleY, scaleZ));
	owner->scaleValues[0] = scaleX; owner->scaleValues[1] = scaleY; owner->scaleValues[2] = scaleZ;
	RealVertexPositions();
	{
		owner->WorldVertexPositions[0] = WorldVerPos[0];
		owner->WorldVertexPositions[1] = WorldVerPos[1];
		owner->WorldVertexPositions[2] = WorldVerPos[4];
		owner->WorldVertexPositions[3] = WorldVerPos[5];
		owner->WorldVertexPositions[4] = WorldVerPos[8];
		owner->WorldVertexPositions[5] = WorldVerPos[9];
		owner->WorldVertexPositions[6] = WorldVerPos[12];
		owner->WorldVertexPositions[7] = WorldVerPos[13];
	}
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, owner->pos, owner->view, owner->scale);
	//EventSystem::Instance()->SendEvent("OnScale",(Listener*)owner);
	if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->Scale(scaleX, scaleY, scaleZ);
		}
	}
}

void Transform::Translate(float x, float y,float z)
{
	if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true) {
				double _x = go->position.x - owner->position.x;
				double _y = go->position.y - owner->position.y;
				double _z = go->position.z - owner->position.z;
				go->GetComponentManager()->GetComponent<Transform>()->Translate(_x + x, _y + y, _z + z);
			}
		}
	}
	position.x = x;
	position.y = y;
	position.z = z;
	owner->pos = translate(glm::mat4(1.f), glm::vec3(position.x, position.y, position.z));
	RealVertexPositions();
	{
		owner->WorldVertexPositions[0] = WorldVerPos[0];
		owner->WorldVertexPositions[1] = WorldVerPos[1];
		owner->WorldVertexPositions[2] = WorldVerPos[4];
		owner->WorldVertexPositions[3] = WorldVerPos[5];
		owner->WorldVertexPositions[4] = WorldVerPos[8];
		owner->WorldVertexPositions[5] = WorldVerPos[9];
		owner->WorldVertexPositions[6] = WorldVerPos[12];
		owner->WorldVertexPositions[7] = WorldVerPos[13];
	}
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, owner->pos, owner->view, owner->scale);
	owner->position.x = position.x;
	owner->position.y = position.y;
	owner->position.z = position.z;
}
