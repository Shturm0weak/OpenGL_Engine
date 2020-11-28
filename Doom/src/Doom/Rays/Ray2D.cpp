#include "../pch.h"
#include "../Render/Renderer.h"
#include "Ray2D.h"

using namespace Doom;

Ray2D::Ray2D(glm::vec2 start, glm::vec2 direction, float length)
	: start(start),length(length)
{
	this->direction = direction;
	Normilize(this->direction);
	this->end = start + (direction * length);
}

bool Ray2D::Raycast(Hit & hit,float length,glm::vec2 start,glm::vec2 direction, std::vector<std::string> ignoreMask)
{
	this->start = start;
	this->direction = direction;
	end = (this->direction * length) + start;
	
	std::vector<glm::vec2> corners;
	std::multimap<RectangleCollider2D*,glm::vec2*> points;
	unsigned int size = Renderer::collision2d.size();

	bool ignoreFlag = false;

	for (unsigned int i = 0; i < size; i++)
	{
		RectangleCollider2D* col = Renderer::collision2d[i];

		unsigned int ignoreMaskSize = ignoreMask.size();
		for (unsigned int i = 0; i < ignoreMaskSize; i++)
		{
			if (ignoreMask[i] == col->GetTag())
				ignoreFlag = true;
		}

		if (ignoreFlag == true) {
			ignoreFlag = false;
			continue;
		}

		if (col->Enable != false) {
			corners.clear();
			corners.push_back(glm::vec2(col->ScaledVerPos[0], col->ScaledVerPos[1]));
			corners.push_back(glm::vec2(col->ScaledVerPos[4], col->ScaledVerPos[5]));
			corners.push_back(glm::vec2(col->ScaledVerPos[8], col->ScaledVerPos[9]));
			corners.push_back(glm::vec2(col->ScaledVerPos[12], col->ScaledVerPos[13]));

			glm::vec2* result = nullptr;
			result = LineIntersect(corners[3] + glm::vec2(col->position.x, col->position.y), corners[0] + glm::vec2(col->position.x, col->position.y), start, end);
			if (result != nullptr)
				points.insert(std::make_pair(col, result));
			result = LineIntersect(corners[0] + glm::vec2(col->position.x, col->position.y), corners[1] + glm::vec2(col->position.x, col->position.y), start, end);
			if (result != nullptr)
				points.insert(std::make_pair(col, result));
			result = LineIntersect(corners[1] + glm::vec2(col->position.x, col->position.y), corners[2] + glm::vec2(col->position.x, col->position.y), start, end);
			if (result != nullptr)
				points.insert(std::make_pair(col, result));
			result = LineIntersect(corners[2] + glm::vec2(col->position.x, col->position.y), corners[3] + glm::vec2(col->position.x, col->position.y), start, end);
			if (result != nullptr)
				points.insert(std::make_pair(col, result));
		}
	}
	int _size = points.size();
	if (_size > 0) {
		float distance = 0;
		float min = 10000000;
		for (auto itr = points.begin(); itr != points.end(); itr++)
		{
			distance = Distance(*itr->second,start);	
			
			if (distance <= min) {
				min = distance;
				hit.point = glm::vec3(itr->second->x, itr->second->y,0);
				hit.Object = itr->first;
			}
			delete itr->second;
		}
		points.clear();
		return true;
	}	
	else
		return false;
}

glm::vec2* Doom::Ray2D::LineIntersect(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 g)
{
	glm::vec2 r = (b - a);
	glm::vec2 s = (g - c);
	float d = r.x * s.y - r.y * s.x;
	float u = ((c.x - a.x) * r.y - (c.y - a.y) * r.x) / d;
	float t = ((c.x - a.x) * s.y - (c.y - a.y) * s.x) / d;
	if (0 <= u && u <= 1 && 0 <= t && t <= 1) {
		glm::vec2* result = new glm::vec2(a + t * r);
		return result;
	}
	return nullptr;
}

float Doom::Ray2D::Distance(glm::vec2 start, glm::vec2 end)
{
	return sqrt(pow((end.x - start.x),2) + pow((end.y - start.y),2));
}

void Doom::Ray2D::Normilize(glm::vec2 & vector)
{
	vector = glm::vec2(vector * (1.f / sqrt(vector.x * vector.x + vector.y * vector.y)));
}

void Doom::Ray2D::Rotate(float angle)
{
	float theta = (-angle * (2 * 3.14159f) / 360.0f);
	direction = glm::vec2(cos(theta) * direction.x - sin(theta) * direction.y,
		sin(theta) * direction.x + cos(theta) * direction.y);
	this->end = start + (direction * length);
}

void Ray2D::SetStart(glm::vec2 start) {
	this->start = start;
}

void Ray2D::SetDirection(glm::vec2 direction) {
	this->direction = direction;
	Normilize(this->direction);
}