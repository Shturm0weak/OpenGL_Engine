#include "../pch.h"
#include "GuiItem.h"

using namespace Doom;

void GuiItem::Scale(float scalex,float scaley)
{
	_scale.x = scalex;
	_scale.y = scaley;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(_scale.x, _scale.y, 0));
}

void GuiItem::Translate(float x, float y)
{
	position.x = x;
	position.y = y;
	pos = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0));
}