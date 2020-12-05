#include "../pch.h"
#include "Line.h"

Line::Line(glm::vec3 start, glm::vec3 end)
{
	m_IsUsingPosMat4 = false;
	this->m_Verteces[0] = start.x;
	this->m_Verteces[1] = start.y;
	this->m_Verteces[2] = start.z;
	this->m_Verteces[3] = end.x;
	this->m_Verteces[4] = end.y;
	this->m_Verteces[5] = end.z;
	this->s_Lines.push_back(this);
}

Line::Line(glm::vec3 start, glm::vec3 direction, float maxLength)
{
	this->m_PosMat4 = translate(glm::mat4(1.f), glm::vec3(start.x, start.y, 0));
	this->m_MaxLength = maxLength;
	this->m_Dir = glm::vec3(direction * (1.f / sqrt(direction.x * direction.x + direction.y * direction.y)));
	this->m_Verteces[2] = this->m_Dir.x * maxLength;
	this->m_Verteces[3] = this->m_Dir.y * maxLength;
	this->s_Lines.push_back(this);
}

Line::Line(glm::vec3 start, float angle, float maxLength)
{
	this->m_Angle = -angle;
	this->m_PosMat4 = translate(glm::mat4(1.f), glm::vec3(start.x, start.y,0));
	this->m_MaxLength = maxLength;
	this->m_ViewMat4 = glm::rotate(glm::mat4(1.f),(-angle * (2 * 3.14159f) / 360.0f), glm::vec3(0,0,1));
	this->m_Verteces[3] = maxLength;
	this->s_Lines.push_back(this);
}

void Line::SetEndPoint(glm::vec3 pos)
{
	m_Verteces[3] = pos[0];
	m_Verteces[4] = pos[1];
	m_Verteces[5] = pos[2];
}

void Line::SetEndPoint(float * pos)
{
	m_Verteces[3] = pos[0];
	m_Verteces[4] = pos[1];
	m_Verteces[5] = pos[2];
}

void Line::SetEndPoint(float x, float y,float z)
{
	m_Verteces[3] = x;
	m_Verteces[4] = y;
	m_Verteces[5] = z;
}

void Line::SetStartPoint(float * pos)
{
	m_Verteces[0] = pos[0];
	m_Verteces[1] = pos[1];
	m_Verteces[2] = pos[2];
	if(m_IsUsingPosMat4)
		this->m_PosMat4 = translate(glm::mat4(1.f), glm::vec3(pos[0], pos[1], pos[2]));
}

void Line::SetStartPoint(glm::vec3 pos)
{
	m_Verteces[0] = pos[0];
	m_Verteces[1] = pos[1];
	m_Verteces[2] = pos[2];
	if (m_IsUsingPosMat4)
		this->m_PosMat4 = translate(glm::mat4(1.f), glm::vec3(pos[0], pos[1], pos[2]));
}

void Line::SetStartPoint(float x, float y,float z)
{
	m_Verteces[0] = x;
	m_Verteces[1] = y;
	m_Verteces[2] = z;
		if (m_IsUsingPosMat4)
	this->m_PosMat4 = translate(glm::mat4(1.f), glm::vec3(x, y, z));
}

void Line::Rotate(float angle)
{
	m_ViewMat4 = glm::rotate(m_ViewMat4, angle * DeltaTime::s_Deltatime, glm::vec3(0, 0, 1));
}
