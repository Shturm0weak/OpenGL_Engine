#include "../pch.h"
#include "Ray.h"

using namespace Doom;

Ray::Ray(glm::vec2 start, glm::vec2 direction, float length)
	: start(start),maxLength(length)
{
	this->direction = glm::vec2(direction * (1.f / sqrt(direction.x * direction.x + direction.y * direction.y)));
	this->end = start;
}

bool Ray::Raycast(Hit & hit,float length)
{
	end = start;
	maxLength = length;
	int doublemaxlength = (maxLength) * (maxLength);
	while ((direction.x * direction.x + direction.y * direction.y) < doublemaxlength) {
		unsigned int size = Renderer2DLayer::collision2d.size();
		for (unsigned int i = 0; i < size; i++)
		{
			Collision* col = (Collision*)&Renderer2DLayer::collision2d[i].get();
			if (col->tag == "Player" || col->Enable == false)
				continue;
			glm::vec2 currentPos = end + direction;
			if ((col->ScaledVerPos[1] + col->position.y  < currentPos.y && col->ScaledVerPos[5] + col->position.y < currentPos.y) && (col->ScaledVerPos[13] + col->position.y > currentPos.y && col->ScaledVerPos[9] + col->position.y > currentPos.y)
				&& (col->ScaledVerPos[0] + col->position.x < currentPos.x && col->ScaledVerPos[12] + col->position.x < currentPos.x) && (col->ScaledVerPos[8] + col->position.x > currentPos.x && col->ScaledVerPos[4] + col->position.x > currentPos.x)) {
				hit.point = currentPos;
				hit.Object = col;
				return true;
			}
		}
		direction *= 1.1f;
	}
	
	return false;
}

void Ray::SetStart(glm::vec2 start) {
	this->start = start;
}

void Ray::SetDirection(glm::vec2 end) {
	this->direction = glm::vec2(end * (1.f / sqrt(end.x * end.x + end.y * end.y)));
}
