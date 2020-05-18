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
	//EventSystem::Instance()->SendEvent("OnMove",(Listener*)owner);
}

//angle in Rad
void Transform::RotateOnce(float theta, glm::vec3 axis) {
	this->angle = theta;
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
}

//angle in Rad
void Transform::Rotate(float theta, glm::vec3 axis) {
	this->angle = theta;
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
}

void Transform::Scale(float scaleX, float scaleY) {
	owner->scale = glm::scale(glm::mat4(1.f), glm::vec3(scaleX, scaleY, 0));
	owner->scaleValues[0] = scaleX; owner->scaleValues[1] = scaleY;
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
}

void Transform::Translate(float x, float y)
{
	position.x = x;
	position.y = y;
	position.z = 0;
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
}
