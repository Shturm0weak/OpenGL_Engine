#include "../pch.h"
#include "ViewPort.h"

using namespace Doom;

ViewPort * Doom::ViewPort::GetInstance()
{
	static ViewPort instance;
	return &instance;
}

void Doom::ViewPort::Update()
{
	GetMousePositionToScreenSpaceImpl();
	GetMousePositionToWorldSpaceImpl();
	GetStaticMousePositionImpl();

	void* tex = reinterpret_cast<void*>(Window::GetCamera().frameBuffer->texture);

	ImGui::Begin("ViewPort", &ViewPort::GetInstance()->toolOpen, ImGuiWindowFlags_NoScrollbar);
	if (ImGui::IsWindowFocused())
		ViewPort::GetInstance()->IsActive = true;
	else
		ViewPort::GetInstance()->IsActive = false;

	if (ImGui::IsWindowHovered())
		ViewPort::GetInstance()->IsHovered = true;
	else
		ViewPort::GetInstance()->IsHovered = false;

	ViewPort::GetInstance()->SetViewPortPos(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	if (ViewPort::GetInstance()->GetSize().x != ImGui::GetWindowSize().x || ViewPort::GetInstance()->GetSize().y != ImGui::GetWindowSize().y) {
		ViewPort::GetInstance()->SetSize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		ViewPort::GetInstance()->viewportResized = true;
	}
	else
		ViewPort::GetInstance()->viewportResized = false;

	ImGui::GetWindowDrawList()->AddImage(tex, ImVec2(ViewPort::GetInstance()->GetViewPortPos().x,
		ViewPort::GetInstance()->GetViewPortPos().y), ImVec2(ViewPort::GetInstance()->GetViewPortPos().x + ViewPort::GetInstance()->GetSize().x, ViewPort::GetInstance()->GetViewPortPos().y + ViewPort::GetInstance()->GetSize().y), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
}

void Doom::ViewPort::GetMousePositionToWorldSpaceImpl()
{
	glfwGetCursorPos(Window::GetWindow(), &cursorPos.x, &cursorPos.y);
	mousePosW.x = ((((cursorPos.x - viewportPos.x) / (Window::GetSize()[0] - 1))) - 0.5) * Window::GetCamera().GetAspectRatio() * 2 * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().x;
	mousePosW.y = (((-((cursorPos.y + (Window::GetSize()[1] - size.y)) / Window::GetSize()[1])) + 0.5)) * 2 * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().y;
}
void Doom::ViewPort::GetStaticMousePositionImpl()
{
	glfwGetCursorPos(Window::GetWindow(), &cursorPos.x, &cursorPos.y);
	staticMousePos.x = ((((cursorPos.x - viewportPos.x) / (Window::GetSize()[0] - 1))) - 0.5) * Window::GetCamera().GetAspectRatio() * WIDTH * 2;
	staticMousePos.y = (((-((cursorPos.y + (Window::GetSize()[1] - size.y)) / Window::GetSize()[1])) + 0.5)) * HEIGHT * 2;
}

void Doom::ViewPort::GetMousePositionToScreenSpaceImpl()
{
	glfwGetCursorPos(Window::GetWindow(), &cursorPos.x, &cursorPos.y);
	mousePosS.x = ((((cursorPos.x - viewportPos.x) / (Window::GetSize()[0] - 1))) - 0.5) * 2 * WIDTH;
	mousePosS.y = (((-((cursorPos.y + (Window::GetSize()[1] - size.y)) / Window::GetSize()[1])) + 0.5)) * 2 * (float)HEIGHT / Window::GetCamera().GetAspectRatio();
}

glm::dvec2 Doom::ViewPort::GetFromWorldToScreenSpaceImpl(glm::vec2 pos)
{
	return glm::dvec2(pos.x / (Window::GetCamera().GetAspectRatio() * Window::GetCamera().GetZoomLevel()) * WIDTH, pos.y / Window::GetCamera().GetZoomLevel() * HEIGHT);
}

glm::dvec2 Doom::ViewPort::GetFromWorldToScreenSpace(float x, float y)
{
	return glm::dvec2(x / (Window::GetCamera().GetAspectRatio() * Window::GetCamera().GetZoomLevel()) * WIDTH, y / (Window::GetCamera().GetZoomLevel() * Window::GetCamera().GetAspectRatio()) * HEIGHT);
}
