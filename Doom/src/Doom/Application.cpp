#include "Application.h"

void Doom::Application::Init()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	switch (type)
	{
	case Doom::TYPE_2D:
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		Window::GetCamera().SetOrthographic((float)width / height);
		break;
	case Doom::TYPE_3D:

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		Window::GetCamera().SetPerspective(1, width, height, 0.1, 100.0);
		break;
	default:
		break;
	}
}
