#include "../pch.h"
#include "Line.h"

Line::Line(glm::vec2 start, glm::vec2 end)
{
	UsePosMat = false;
	this->mesh2D[0] = start.x;
	this->mesh2D[1] = start.y;
	this->mesh2D[2] = end.x;
	this->mesh2D[3] = end.y;
	this->lines.push_back(this);
}

Line::Line(glm::vec2 start, glm::vec2 direction, float maxLength)
{
	this->pos = translate(glm::mat4(1.f), glm::vec3(start.x, start.y, 0));
	this->maxLength = maxLength;
	this->direction = glm::vec2(direction * (1.f / sqrt(direction.x * direction.x + direction.y * direction.y)));
	this->mesh2D[2] = this->direction.x * maxLength;
	this->mesh2D[3] = this->direction.y * maxLength;
	this->lines.push_back(this);
}

Line::Line(glm::vec2 start, float angle, float maxLength)
{
	this->angle = -angle;
	this->pos = translate(glm::mat4(1.f), glm::vec3(start.x, start.y,0));
	this->maxLength = maxLength;
	this->view = glm::rotate(glm::mat4(1.f),(-angle * (2 * 3.14159f) / 360.0f), glm::vec3(0,0,1));
	this->mesh2D[3] = maxLength;
	this->lines.push_back(this);
}

void Line::SetEndPoint(float * pos)
{
	mesh2D[2] = pos[0];
	mesh2D[3] = pos[1];
}

void Line::SetEndPoint(float x, float y)
{
	mesh2D[2] = x;
	mesh2D[3] = y;
}

void Line::SetStartPoint(float * pos)
{
	mesh2D[0] = pos[0];
	mesh2D[1] = pos[1];
	if(UsePosMat)
		this->pos = translate(glm::mat4(1.f), glm::vec3(pos[0], pos[1], 0));
}

void Line::SetStartPoint(float x, float y)
{
	mesh2D[0] = x;
	mesh2D[1] = y;
		if (UsePosMat)
	this->pos = translate(glm::mat4(1.f), glm::vec3(x, y, 0));
}

void Line::Rotate(float angle)
{
	view = glm::rotate(view, angle * DeltaTime::deltatime, glm::vec3(0, 0, 1));
}
