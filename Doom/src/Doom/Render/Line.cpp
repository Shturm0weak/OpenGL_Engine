#include "../pch.h"
#include "Line.h"

Line::Line(glm::vec3 start, glm::vec3 end)
{
	UsePosMat = false;
	this->mesh2D[0] = start.x;
	this->mesh2D[1] = start.y;
	this->mesh2D[2] = start.z;
	this->mesh2D[3] = end.x;
	this->mesh2D[4] = end.y;
	this->mesh2D[5] = end.z;
	this->lines.push_back(this);
}

Line::Line(glm::vec3 start, glm::vec3 direction, float maxLength)
{
	this->pos = translate(glm::mat4(1.f), glm::vec3(start.x, start.y, 0));
	this->maxLength = maxLength;
	this->direction = glm::vec3(direction * (1.f / sqrt(direction.x * direction.x + direction.y * direction.y)));
	this->mesh2D[2] = this->direction.x * maxLength;
	this->mesh2D[3] = this->direction.y * maxLength;
	this->lines.push_back(this);
}

Line::Line(glm::vec3 start, float angle, float maxLength)
{
	this->angle = -angle;
	this->pos = translate(glm::mat4(1.f), glm::vec3(start.x, start.y,0));
	this->maxLength = maxLength;
	this->view = glm::rotate(glm::mat4(1.f),(-angle * (2 * 3.14159f) / 360.0f), glm::vec3(0,0,1));
	this->mesh2D[3] = maxLength;
	this->lines.push_back(this);
}

void Line::SetEndPoint(glm::vec3 pos)
{
	mesh2D[3] = pos[0];
	mesh2D[4] = pos[1];
	mesh2D[5] = pos[2];
}

void Line::SetEndPoint(float * pos)
{
	mesh2D[3] = pos[0];
	mesh2D[4] = pos[1];
	mesh2D[5] = pos[2];
}

void Line::SetEndPoint(float x, float y,float z)
{
	mesh2D[3] = x;
	mesh2D[4] = y;
	mesh2D[5] = z;
}

void Line::SetStartPoint(float * pos)
{
	mesh2D[0] = pos[0];
	mesh2D[1] = pos[1];
	mesh2D[2] = pos[2];
	if(UsePosMat)
		this->pos = translate(glm::mat4(1.f), glm::vec3(pos[0], pos[1], pos[2]));
}

void Line::SetStartPoint(glm::vec3 pos)
{
	mesh2D[0] = pos[0];
	mesh2D[1] = pos[1];
	mesh2D[2] = pos[2];
	if (UsePosMat)
		this->pos = translate(glm::mat4(1.f), glm::vec3(pos[0], pos[1], pos[2]));
}

void Line::SetStartPoint(float x, float y,float z)
{
	mesh2D[0] = x;
	mesh2D[1] = y;
	mesh2D[2] = z;
		if (UsePosMat)
	this->pos = translate(glm::mat4(1.f), glm::vec3(x, y, z));
}

void Line::Rotate(float angle)
{
	view = glm::rotate(view, angle * DeltaTime::deltatime, glm::vec3(0, 0, 1));
}
