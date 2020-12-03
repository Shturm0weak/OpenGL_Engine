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

	void* tex = reinterpret_cast<void*>(Window::GetCamera().m_FrameBufferColor->texture);

	ImGui::Begin("ViewPort", &ViewPort::GetInstance()->m_IsToolOpen, ImGuiWindowFlags_NoScrollbar);
	if (ImGui::IsWindowFocused())
		ViewPort::GetInstance()->m_IsActive = true;
	else
		ViewPort::GetInstance()->m_IsActive = false;

	if (ImGui::IsWindowHovered())
		ViewPort::GetInstance()->m_IsHovered = true;
	else
		ViewPort::GetInstance()->m_IsHovered = false;

	ViewPort::GetInstance()->SetViewPortPos(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	if (ViewPort::GetInstance()->GetSize().x != ImGui::GetWindowSize().x || ViewPort::GetInstance()->GetSize().y != ImGui::GetWindowSize().y) {
		ViewPort::GetInstance()->SetSize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		ViewPort::GetInstance()->m_IsViewportResized = true;
	}
	else
		ViewPort::GetInstance()->m_IsViewportResized = false;

	m_MouseDragDelta = GetMousePositionToScreenSpace() - m_PrevMousePos;
	m_PrevMousePos = GetMousePositionToScreenSpace();
	ImGui::GetWindowDrawList()->AddImage(tex, ImVec2(ViewPort::GetInstance()->GetViewPortPos().x,
		ViewPort::GetInstance()->GetViewPortPos().y), ImVec2(ViewPort::GetInstance()->GetViewPortPos().x + ViewPort::GetInstance()->GetSize().x, ViewPort::GetInstance()->GetViewPortPos().y + ViewPort::GetInstance()->GetSize().y), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
}

void Doom::ViewPort::GetMousePositionToWorldSpaceImpl()
{
	glfwGetCursorPos(Window::GetWindow(), &m_CursorPos.x, &m_CursorPos.y);
	m_MousePosW.x = ((((m_CursorPos.x - m_ViewportPos.x) / (Window::GetSize()[0] - 1))) - 0.5) * Window::GetCamera().GetAspectRatio() * 2 * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().x;
	m_MousePosW.y = (((-((m_CursorPos.y + (Window::GetSize()[1] - m_Size.y)) / Window::GetSize()[1])) + 0.5)) * 2 * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().y;
}
void Doom::ViewPort::GetStaticMousePositionImpl()
{
	glfwGetCursorPos(Window::GetWindow(), &m_CursorPos.x, &m_CursorPos.y);
	m_StaticMousePos.x = ((((m_CursorPos.x - m_ViewportPos.x) / (Window::GetSize()[0] - 1))) - 0.5) * Window::GetCamera().GetAspectRatio() * g_Width * 2;
	m_StaticMousePos.y = (((-((m_CursorPos.y + (Window::GetSize()[1] - m_Size.y)) / Window::GetSize()[1])) + 0.5)) * g_Height * 2;
}

void Doom::ViewPort::GetMousePositionToScreenSpaceImpl()
{
	glfwGetCursorPos(Window::GetWindow(), &m_CursorPos.x, &m_CursorPos.y);
	m_MousePosS.x = ((((m_CursorPos.x - m_ViewportPos.x) / (Window::GetSize()[0] - 1))) - 0.5) * 2 * g_Width;
	m_MousePosS.y = (((-((m_CursorPos.y + (Window::GetSize()[1] - m_Size.y)) / Window::GetSize()[1])) + 0.5)) * 2 * (float)g_Height / Window::GetCamera().GetAspectRatio();
}

glm::dvec2 Doom::ViewPort::GetFromWorldToScreenSpaceImpl(glm::vec2 pos)
{
	return glm::dvec2(pos.x / (Window::GetCamera().GetAspectRatio() * Window::GetCamera().GetZoomLevel()) * g_Width, pos.y / Window::GetCamera().GetZoomLevel() * g_Height);
}

glm::dvec2 Doom::ViewPort::GetFromWorldToScreenSpace(float x, float y)
{
	return glm::dvec2(x / (Window::GetCamera().GetAspectRatio() * Window::GetCamera().GetZoomLevel()) * g_Width, y / (Window::GetCamera().GetZoomLevel() * Window::GetCamera().GetAspectRatio()) * g_Height);
}
