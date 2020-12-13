#include "AimTrainer.h"
#include <iostream>
#include "Core/ViewPort.h"
#include "Rays/Ray3D.h"
#include "Objects/Line.h"
#include "Text/Gui.h"

using namespace Doom;

void Doom::AimTrainer::RayCast()
{
	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1)) {
		Ray3D::Hit hit;
		glm::vec3 forward = Window::GetCamera().forwardV;
		glm::vec3 dir = glm::vec3(-forward.x, forward.y, -forward.z);
		Ray3D::Normilize(dir);
		std::map<float, CubeCollider3D*> intersects = Ray3D::RayCast(Window::GetCamera().GetPosition(), dir, &hit, 10000, false);
		//new Line(Window::GetCamera().GetPosition(), Window::GetCamera().GetPosition() + dir * 100.f);
		if (hit.m_Object != nullptr) {
			hit.m_Object->GetOwnerOfComponent()->GetComponent<Renderer3D>()->m_Color = COLORS::Blue;
		}
	}
}

void Doom::AimTrainer::CameraMovement()
{
	glm::dvec2 delta = ViewPort::GetInstance()->GetMouseDragDelta();
	delta *= 0.2;
	glm::vec3 rot = Window::GetCamera().GetRotation();
	if (rot.y > glm::two_pi<float>() || rot.y < -glm::two_pi<float>())
		rot.y = 0;
	if (rot.x > glm::two_pi<float>() || rot.x < -glm::two_pi<float>())
		rot.x = 0;
	Window::GetCamera().SetRotation(glm::vec3((rot.x + delta.y * (2 * 3.14159f) / 360.0f), (rot.y - delta.x * (2 * 3.14159f) / 360.0f), 0));	
}

void AimTrainer::OnStart()
{
	Texture::Create("src/AimTrainer/CrossHairs/crossHair.png");
}

void AimTrainer::OnUpdate()
{
	if (Input::IsKeyPressed(Keycode::KEY_ESCAPE)) {
		Window::ShowCursor();
	}
	else if (Input::IsKeyPressed(Keycode::KEY_BACKSPACE)) {
		Window::DisableCursor();
	}
	CameraMovement();
	RayCast();
}

void AimTrainer::OnClose()
{
}

void Doom::AimTrainer::OnGuiRender()
{
	Gui::GetInstance()->Image(0, 0, 50, 50, Texture::Get("src/AimTrainer/CrossHairs/crossHair.png"));
}
