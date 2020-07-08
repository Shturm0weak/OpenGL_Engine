#include "../pch.h"
#include "Transform.h"
#include "../Core/EventSystem.h"
#include "../Objects2D/GameObject.h"
#include "../Render/ViewPort.h"
#include "../Core/Timer.h"

using namespace Doom;

Transform::Transform() {
	SetType("Transform");
}

void Transform::init() {
	position.x = owner->position.x;
	position.y = owner->position.y;
	position.z = owner->position.z;
}

void Transform::RealVertexPositions()
{
	if (prevPosition.x + prevPosition.y + prevPosition.z != position.x + position.y + position.z || prevAngle != angleDeg) {
		prevAngle == angleDeg;
		prevPosition = position;
		sr->Update(glm::vec3(position.x,position.y,position.z));
	}
}

void Transform::Move(float speedX,float speedY,float speedZ) {
	sr = owner->GetComponentManager()->GetComponent<Irenderer>();

	position.x += speedX * DeltaTime::GetDeltaTime();
	position.y += speedY * DeltaTime::GetDeltaTime();
	position.z += speedZ * DeltaTime::GetDeltaTime();
	RealVertexPositions();
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr) {
		col->UpdateCollision(position.x, position.y, sr->pos, sr->view, sr->scale);
	}
	owner->position.x = position.x;
	owner->position.y = position.y;
	owner->position.z = position.z;
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

void Transform::RotateOnce(float x, float y, float z,bool isRad) {
	sr = owner->GetComponentManager()->GetComponent<Irenderer>();
	if (isRad) {
		//this->angleDeg = (-theta * 360.0f) / (2 * 3.14159f);
		//this->angleRad = theta;
	}
	else {
		//this->angleRad = (-theta * (2 * 3.14159f) / 360.0f);
		rotation.x = (-x * (2 * 3.14159f) / 360.0f);
		rotation.y = (-y * (2 * 3.14159f) / 360.0f);
		rotation.z = (-z * (2 * 3.14159f) / 360.0f);
		//this->angleDeg = theta;
	}
	sr->view = glm::mat4(1.0f);
	sr->view = glm::rotate(sr->view, rotation.x, glm::vec3(1, 0, 0));
	sr->view = glm::rotate(sr->view, rotation.y, glm::vec3(0, 1, 0));
	sr->view = glm::rotate(sr->view, rotation.z, glm::vec3(0, 0, 1));
	RealVertexPositions();
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, sr->pos, sr->view, sr->scale);
	//EventSystem::Instance()->SendEvent("OnRotate", (Listener*)owner);
	if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(rotation.x,rotation.y,rotation.z);
		}
	}
}

void Doom::Transform::RotateOnce(glm::vec3 a, glm::vec3 axis)
{
	sr = owner->GetComponentManager()->GetComponent<Irenderer>();

	glm::vec3 b = glm::vec3(0,1,0);
	this->angleRad = acosf((a.x * b.x + a.y * b.y + a.z * b.z)/((sqrtf(a.x * a.x + a.y * a.y + a.z * a.z) * (sqrtf(b.x * b.x + b.y * b.y + b.z * b.z)))));
	if (ViewPort::GetInstance()->GetMousePositionToWorldSpace().x > position.x)
		this->angleRad = -this->angleRad;
	this->angleDeg = (-this->angleRad * 360.0f) / (2 * 3.14159f);
	sr->view = glm::mat4(1.0f);
	sr->view = glm::rotate(sr->view, angleRad, axis);
	axis *= angleRad;
	RealVertexPositions();
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, sr->pos, sr->view, sr->scale);
	//EventSystem::Instance()->SendEvent("OnRotate", (Listener*)owner);
	if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(axis.x,axis.y,axis.z);
		}
	}
}

void Transform::Rotate(float x, float y, float z) {
	sr = owner->GetComponentManager()->GetComponent<Irenderer>();
	rotation.x = (-x * (2 * 3.14159f) / 360.0f);
	rotation.y = (-y * (2 * 3.14159f) / 360.0f);
	rotation.z = (-z * (2 * 3.14159f) / 360.0f);
	sr->view = glm::rotate(sr->view, rotation.x * DeltaTime::GetDeltaTime(), glm::vec3(1, 0, 0));
	sr->view = glm::rotate(sr->view, rotation.y * DeltaTime::GetDeltaTime(), glm::vec3(0, 1, 0));
	sr->view = glm::rotate(sr->view, rotation.z * DeltaTime::GetDeltaTime(), glm::vec3(0, 0, 1));
	RealVertexPositions();
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, sr->pos, sr->view, sr->scale);
	//EventSystem::Instance()->SendEvent("OnRotate",(Listener*)owner);
	if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(rotation.x, rotation.y, rotation.z);
		}
	}
}

void Transform::Scale(float scaleX, float scaleY,float scaleZ) {
	sr = owner->GetComponentManager()->GetComponent<Irenderer>();

	sr->scale = glm::scale(glm::mat4(1.f), glm::vec3(scaleX, scaleY, scaleZ));
	owner->scaleValues[0] = scaleX; owner->scaleValues[1] = scaleY; owner->scaleValues[2] = scaleZ;
	RealVertexPositions();
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, sr->pos, sr->view, sr->scale);
	//EventSystem::Instance()->SendEvent("OnScale",(Listener*)owner);
	//if (owner->Enable) {
		unsigned int size = owner->GetChilds().size();
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = static_cast<GameObject*>(owner->GetChilds()[i]);
			if (go->Enable == true)
				go->GetComponentManager()->GetComponent<Transform>()->Scale(scaleX, scaleY, scaleZ);
		}
	//}
}

void Transform::Translate(float x, float y,float z)
{
	sr = owner->GetComponentManager()->GetComponent<Irenderer>();

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
	sr->pos = translate(glm::mat4(1.f), glm::vec3(position.x, position.y, position.z));
	RealVertexPositions();
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, sr->pos, sr->view, sr->scale);
	owner->position.x = position.x;
	owner->position.y = position.y;
	owner->position.z = position.z;
}
