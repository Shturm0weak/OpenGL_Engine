#include "../pch.h"
#include "ViewPort.h"
#include "Editor.h"
#include "ImGuizmo/ImGuizmo.h"
#include "Core/Utils.h"
#include "Text/Gui.h"

using namespace Doom;

ViewPort& Doom::ViewPort::GetInstance()
{
	static ViewPort instance;
	return instance;
}

void Doom::ViewPort::Update()
{
	ViewPort& viewport = ViewPort::GetInstance();
	void* tex = reinterpret_cast<void*>(Window::GetInstance().m_FrameBufferMainImage->m_Textures[0]);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("ViewPort", &viewport.m_IsToolOpen, ImGuiWindowFlags_NoScrollbar);
	GetMousePositionToScreenSpaceImpl();
	GetMousePositionToWorldSpaceImpl();
	GetStaticMousePositionImpl();
	if (ImGui::IsWindowFocused())
		viewport.m_IsActive = true;
	else
		viewport.m_IsActive = false;

	if (ImGui::IsWindowHovered()) 
		viewport.m_IsHovered = true;
	else
		viewport.m_IsHovered = false;
	if (viewport.GetSize().x != ImGui::GetWindowSize().x || viewport.GetSize().y != ImGui::GetWindowSize().y) 
	{
		viewport.SetSize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		viewport.m_IsViewportResized = true;
	}
	else 
		viewport.m_IsViewportResized = false;

	viewport.SetViewPortPos(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);

	m_MouseDragDelta = GetMousePositionToScreenSpace() - m_PrevMousePos;
	m_PrevMousePos = GetMousePositionToScreenSpace();

	ImGui::Image(tex, ImVec2(m_Size.x, m_Size.y), ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::GetWindowDrawList()->AddImage(tex, ImVec2(viewport.GetViewPortPos().x,
	//viewport.GetViewPortPos().y), ImVec2(viewport.GetViewPortPos().x + viewport.GetSize().x, viewport.GetViewPortPos().y + viewport.GetSize().y), ImVec2(0, 1), ImVec2(1, 0));

	//Need to fix Rotation doesn't change orientation of gizmos
	if (Editor::GetInstance().go != nullptr) 
	{
		Transform* tr = &(Editor::GetInstance().go->m_Transform);
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(viewport.GetViewPortPos().x,
			viewport.GetViewPortPos().y,
			windowWidth,
			windowHeight);
		glm::mat4 cameraProjection = Window::GetInstance().GetCamera().GetProjectionMatrix();
		glm::mat4 cameraView = Window::GetInstance().GetCamera().GetViewMatrix();
		glm::mat4 transform = tr->GetTransform();
		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
			(ImGuizmo::OPERATION)m_GizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(transform));

		if(ImGuizmo::IsUsing())
		{
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

void Doom::ViewPort::Resize()
{
	if (ViewPort::GetInstance().m_IsViewportResized == false) return;
	Camera& camera = Window::GetInstance().GetCamera();
	camera.m_Ratio = m_Size.x / m_Size.y;
	switch (camera.m_Type)
	{
	case Doom::Camera::CameraTypes::ORTHOGRAPHIC:
		camera.SetOrthographic(camera.m_Ratio);
		break;
	case Doom::Camera::CameraTypes::PERSPECTIVE:
		camera.SetPerspective(camera.m_Fov, m_Size.x, m_Size.y, camera.m_Znear, camera.m_Zfar);
		break;
	default:
		break;
	}
	camera.RecalculateViewMatrix();
	Gui::GetInstance().RecalculateProjectionMatrix();
	Window& window = Window::GetInstance();
	int* size = window.GetSize();

	Doom::Texture::s_TexParameters[2] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER };
	Doom::Texture::s_TexParameters[3] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER };
	window.m_FrameBufferOutline->Resize(size[0], size[1]);

	Doom::Texture::s_TexParameters[2] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE };
	Doom::Texture::s_TexParameters[3] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE };
	window.m_FrameBufferColor  ->Resize(size[0], size[1]);
	window.m_FrameBufferBlur[0]->Resize(size[0], size[1]);
	window.m_FrameBufferBlur[1]->Resize(size[0], size[1]);
	window.m_FrameBufferMainImage->Resize(size[0], size[1]);
}

void Doom::ViewPort::GetMousePositionToWorldSpaceImpl()
{
	Camera& camera = Window::GetInstance().GetCamera();
	int* size = Window::GetInstance().GetSize();
	glfwGetCursorPos(Window::GetInstance().GetWindow(), &m_CursorPos.x, &m_CursorPos.y);
	m_MousePosW.x = ((((m_CursorPos.x - m_ViewportPos.x) / (m_Size[0] - 1))) - 0.5) * 2 * camera.m_Ratio * camera.m_ZoomLevel + camera.GetPosition().x;
	m_MousePosW.y = ((-((m_CursorPos.y - m_ViewportPos.y) * (size[1] / m_Size.y)) / size[1]) + 0.5) * 2 * camera.m_ZoomLevel + camera.GetPosition().y;
}
void Doom::ViewPort::GetStaticMousePositionImpl()
{
	Camera& camera = Window::GetInstance().GetCamera();
	int* size = Window::GetInstance().GetSize();
	glfwGetCursorPos(Window::GetInstance().GetWindow(), &m_CursorPos.x, &m_CursorPos.y);
	//For flattened screen!!!
	m_StaticMousePos.x = ((((m_CursorPos.x - m_ViewportPos.x) / (m_Size[0] - 1))) - 0.5) * camera.m_Ratio * g_ScaleUI * 2;
	m_StaticMousePos.y = ((-((m_CursorPos.y - m_ViewportPos.y) * (size[1] / m_Size.y)) / size[1]) + 0.5) * g_ScaleUI * 2;
	//For normal screen!!!
	//m_StaticMousePos.x = ((((m_CursorPos.x - m_ViewportPos.x) / (Window::GetSize()[0] - 1))) - 0.5) * Window::GetCamera().GetAspectRatio() * g_Width * 2;
	//m_StaticMousePos.y = (((-((m_CursorPos.y + (Window::GetSize()[1] - m_Size.y)) / Window::GetSize()[1])) + 0.5)) * g_Height * 2;
}

void Doom::ViewPort::GetMousePositionToScreenSpaceImpl()
{
	Camera& camera = Window::GetInstance().GetCamera();
	int* size = Window::GetInstance().GetSize();
	glfwGetCursorPos(Window::GetInstance().GetWindow(), &m_CursorPos.x, &m_CursorPos.y);
	m_MousePosS.x = ((((m_CursorPos.x - m_ViewportPos.x) / (m_Size[0] - 1))) - 0.5) * 2 * g_ScaleUI;
	m_MousePosS.y = ((-((m_CursorPos.y - m_ViewportPos.y) * (size[1] / m_Size.y)) / size[1]) + 0.5) * 2 * (float)g_ScaleUI / camera.m_Ratio;
}

void Doom::ViewPort::RecalculateMouseCoords()
{
	GetMousePositionToScreenSpaceImpl();
	m_PrevMousePos = GetMousePositionToScreenSpace();
	m_MouseDragDelta = GetMousePositionToScreenSpace() - m_PrevMousePos;
}

glm::dvec2 Doom::ViewPort::GetFromWorldToScreenSpaceImpl(glm::vec2 pos)
{
	Camera& camera = Window::GetInstance().GetCamera();
	return glm::dvec2(pos.x / (camera.m_Ratio * camera.m_ZoomLevel) * g_ScaleUI, pos.y / camera.m_ZoomLevel * g_ScaleUI);
}

glm::dvec2 Doom::ViewPort::GetFromWorldToScreenSpace(float x, float y)
{
	Camera& camera = Window::GetInstance().GetCamera();
	return glm::dvec2(x / (camera.m_Ratio * camera.m_ZoomLevel) * g_ScaleUI, y / (camera.m_ZoomLevel * camera.m_Ratio) * g_ScaleUI);
}
