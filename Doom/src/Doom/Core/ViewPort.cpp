#include "../pch.h"
#include "ViewPort.h"
#include "Editor.h"
#include "ImGuizmo/ImGuizmo.h"
#include "Core/Utils.h"

using namespace Doom;

ViewPort* Doom::ViewPort::GetInstance()
{
	static ViewPort instance;
	return &instance;
}

void Doom::ViewPort::Update()
{
	void* tex = reinterpret_cast<void*>(Window::GetCamera().m_FrameBufferColor->m_Textures[0]);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("ViewPort", &ViewPort::GetInstance()->m_IsToolOpen, ImGuiWindowFlags_NoScrollbar);
	GetMousePositionToScreenSpaceImpl();
	GetMousePositionToWorldSpaceImpl();
	GetStaticMousePositionImpl();
	if (ImGui::IsWindowFocused())
		ViewPort::GetInstance()->m_IsActive = true;
	else
		ViewPort::GetInstance()->m_IsActive = false;

	if (ImGui::IsWindowHovered())
		ViewPort::GetInstance()->m_IsHovered = true;
	else
		ViewPort::GetInstance()->m_IsHovered = false;
	if (ViewPort::GetInstance()->GetSize().x != ImGui::GetWindowSize().x || ViewPort::GetInstance()->GetSize().y != ImGui::GetWindowSize().y) {
		ViewPort::GetInstance()->SetSize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		ViewPort::GetInstance()->m_IsViewportResized = true;
	}
	else
		ViewPort::GetInstance()->m_IsViewportResized = false;

	ViewPort::GetInstance()->SetViewPortPos(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);

	m_MouseDragDelta = GetMousePositionToScreenSpace() - m_PrevMousePos;
	m_PrevMousePos = GetMousePositionToScreenSpace();

	ImGui::Image(tex, ImVec2(m_Size.x, m_Size.y), ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::GetWindowDrawList()->AddImage(tex, ImVec2(ViewPort::GetInstance()->GetViewPortPos().x,
	//ViewPort::GetInstance()->GetViewPortPos().y), ImVec2(ViewPort::GetInstance()->GetViewPortPos().x + ViewPort::GetInstance()->GetSize().x, ViewPort::GetInstance()->GetViewPortPos().y + ViewPort::GetInstance()->GetSize().y), ImVec2(0, 1), ImVec2(1, 0));

	//Need to fix Rotation doesn't change orientation of gizmos
	if (Editor::GetInstance()->go != nullptr) {
		Transform* tr = Editor::GetInstance()->go->GetComponent<Transform>();
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ViewPort::GetInstance()->GetViewPortPos().x,
			ViewPort::GetInstance()->GetViewPortPos().y,
			windowWidth,
			windowHeight);
		glm::mat4 cameraProjection = Window::GetCamera().GetProjectionMatrix();
		glm::mat4 cameraView = Window::GetCamera().GetViewMatrix();
		glm::mat4 transform = tr->GetTransform();
		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
			(ImGuizmo::OPERATION)m_GizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(transform));

		if(ImGuizmo::IsUsing()){
			glm::vec3 translation, rotation, scale;
			Utils::DecomposeTransform(transform, translation, rotation, scale);
			glm::vec3 deltaRotation = (rotation) - (tr->GetRotation());
			tr->m_Rotation += deltaRotation;
			tr->Translate(translation);
			tr->Scale(scale);
			tr->RotateOnce(tr->m_Rotation, true);
		}
	}
	ImGui::PopStyleVar();
	ImGui::End();
}

void Doom::ViewPort::GetMousePositionToWorldSpaceImpl()
{
	glfwGetCursorPos(Window::GetWindow(), &m_CursorPos.x, &m_CursorPos.y);
	m_MousePosW.x = ((((m_CursorPos.x - m_ViewportPos.x) / (m_Size[0] - 1))) - 0.5) * Window::GetCamera().GetAspectRatio() * 2 * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().x;
	m_MousePosW.y = ((-((m_CursorPos.y - m_ViewportPos.y) * (Window::GetSize()[1] / m_Size.y)) / Window::GetSize()[1]) + 0.5) * 2 * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().y;
}
void Doom::ViewPort::GetStaticMousePositionImpl()
{
	glfwGetCursorPos(Window::GetWindow(), &m_CursorPos.x, &m_CursorPos.y);
	//For flattened screen!!!
	m_StaticMousePos.x = ((((m_CursorPos.x - m_ViewportPos.x) / (m_Size[0] - 1))) - 0.5) * Window::GetCamera().GetAspectRatio() * g_Width * 2;
	m_StaticMousePos.y = ((-((m_CursorPos.y - m_ViewportPos.y) * (Window::GetSize()[1] / m_Size.y)) / Window::GetSize()[1]) + 0.5) * g_Height * 2;
	//For normal screen!!!
	//m_StaticMousePos.x = ((((m_CursorPos.x - m_ViewportPos.x) / (Window::GetSize()[0] - 1))) - 0.5) * Window::GetCamera().GetAspectRatio() * g_Width * 2;
	//m_StaticMousePos.y = (((-((m_CursorPos.y + (Window::GetSize()[1] - m_Size.y)) / Window::GetSize()[1])) + 0.5)) * g_Height * 2;
}

void Doom::ViewPort::GetMousePositionToScreenSpaceImpl()
{
	glfwGetCursorPos(Window::GetWindow(), &m_CursorPos.x, &m_CursorPos.y);
	m_MousePosS.x = ((((m_CursorPos.x - m_ViewportPos.x) / (Window::GetSize()[0] - 1))) - 0.5) * 2 * g_Width;
	m_MousePosS.y = (((-((m_CursorPos.y + (Window::GetSize()[1] - m_Size.y)) / Window::GetSize()[1])) + 0.5)) * 2 * (float)g_Height / Window::GetCamera().GetAspectRatio();
}

void Doom::ViewPort::RecalculateMouseCoords()
{
	GetMousePositionToScreenSpaceImpl();
	m_PrevMousePos = GetMousePositionToScreenSpace();
	m_MouseDragDelta = GetMousePositionToScreenSpace() - m_PrevMousePos;
}

glm::dvec2 Doom::ViewPort::GetFromWorldToScreenSpaceImpl(glm::vec2 pos)
{
	return glm::dvec2(pos.x / (Window::GetCamera().GetAspectRatio() * Window::GetCamera().GetZoomLevel()) * g_Width, pos.y / Window::GetCamera().GetZoomLevel() * g_Height);
}

glm::dvec2 Doom::ViewPort::GetFromWorldToScreenSpace(float x, float y)
{
	return glm::dvec2(x / (Window::GetCamera().GetAspectRatio() * Window::GetCamera().GetZoomLevel()) * g_Width, y / (Window::GetCamera().GetZoomLevel() * Window::GetCamera().GetAspectRatio()) * g_Height);
}
