#include "../pch.h"
#include "Transform.h"
#include "../Core/EventSystem.h"
#include "../Objects2D/GameObject.h"

using namespace Doom;

Transform::Transform() {
	SetType("Transform");
}

void Transform::init() {
	position.x = owner->position.x;
	position.y = owner->position.y;
	position.z = 0;
}

void Transform::Move(float speedX,float speedY,float speedZ) {
	position.x += speedX * DeltaTime::GetDeltaTime();
	position.y += speedY * DeltaTime::GetDeltaTime();
	position.z += speedZ * DeltaTime::GetDeltaTime();
	owner->pos = translate(glm::mat4(1.f), glm::vec3(position.x, position.y, position.z));
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr) {
		col->UpdateCollision(position.x, position.y, owner->pos, owner->view, owner->scale);
	}
	owner->position.x = position.x;
	owner->position.y = position.y;
	//EventSystem::Instance()->SendEvent("OnMove",(Listener*)owner);
}

//angle in Rad
void Transform::RotateOnce(float angle, glm::vec3 axis) {
	this->angle = angle;
	owner->view = glm::mat4(1.0f);
	owner->view = glm::rotate(owner->view, angle, axis);
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, owner->pos, owner->view, owner->scale);
	//EventSystem::Instance()->SendEvent("OnRotate", (Listener*)owner);
}

//angle in Rad
void Transform::Rotate(float angle, glm::vec3 axis) {
	owner->view = glm::rotate(owner->view, angle * DeltaTime::GetDeltaTime(), axis);
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, owner->pos, owner->view, owner->scale);
	//EventSystem::Instance()->SendEvent("OnRotate",(Listener*)owner);
}

void Transform::Scale(float scaleX, float scaleY) {
	owner->scale = glm::scale(glm::mat4(1.f), glm::vec3(scaleX, scaleY, 0));
	owner->scaleValues[0] = scaleX; owner->scaleValues[1] = scaleY;
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, owner->pos, owner->view, owner->scale);
	//EventSystem::Instance()->SendEvent("OnScale",(Listener*)owner);
}

void Transform::Translate(float x, float y)
{
	position.x = x;
	position.y = y;
	position.z = 0;
	owner->pos = translate(glm::mat4(1.f), glm::vec3(position.x, position.y, position.z));
	if (col == nullptr) {
		col = owner->component_manager->GetComponent<Collision>();
	}
	if (col != nullptr)
		col->UpdateCollision(position.x, position.y, owner->pos, owner->view, owner->scale);
	owner->position.x = position.x;
	owner->position.y = position.y;
}
