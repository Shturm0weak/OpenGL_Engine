#include "../pch.h"
#include "Camera.h"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/glm/gtc/type_ptr.hpp"
#include "../vendor/glm/gtx/rotate_vector.hpp"
#include "../Core/Editor.h"
#include "../Core/ViewPort.h"
#include "../Text/Gui.h"

using namespace Doom;

Camera::Camera()
{
}

Doom::Camera::~Camera()
{

}

void Camera::RecalculateViewMatrix()
{
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), m_Roll, glm::vec3(0, 0, 1))
					* glm::rotate(glm::mat4(1.0f), m_Yaw, glm::vec3(0, 1, 0))
					* glm::rotate(glm::mat4(1.0f), m_Pitch, glm::vec3(1, 0, 0));
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)
		* rot;

	float cosPitch = cos(m_Pitch);

	m_Back.z = cos(m_Yaw) * cosPitch;
	m_Back.x = sin(m_Yaw) * cosPitch;
	m_Back.y = sin(m_Pitch);

	m_Back = Utils::Normalize(m_Back);
	m_Up = Utils::Normalize(glm::vec3(m_ViewMat4[0][1], m_ViewMat4[1][1], m_ViewMat4[2][1]));
	m_Forward = Utils::Normalize(glm::vec3(-m_Back.x, m_Back.y, -m_Back.z));
	m_Right = Utils::Normalize(glm::cross(m_Forward, m_Up));

	m_ViewMat4 = glm::inverse(transform);
	m_ViewProjectionMat4 = m_ProjectionMat4 * m_ViewMat4;
}

void Doom::Camera::MovePosition(const glm::vec3& position)
{
	m_Position += position;
	RecalculateViewMatrix();
}

void Doom::Camera::SetPosition(const glm::vec3& position)
{
	m_Position = position;
	RecalculateViewMatrix();
}

void Doom::Camera::SetPerspective(float fovy, float width, float height, float znear, float zfar)
{
	m_Ratio = width / height;
	m_CameraProjParams[0] = -width;
	m_CameraProjParams[1] = width;
	m_CameraProjParams[2] = height;
	m_CameraProjParams[3] = -height;
	m_Zfar = zfar;
	m_Znear = znear;
	m_Fov = fovy;
	m_ProjectionMat4 = glm::perspective(fovy, width / height, znear, zfar);
	m_Type = CameraTypes::PERSPECTIVE;
}

void Doom::Camera::SetFov(float fov)
{
	m_ProjectionMat4 = glm::perspective(m_Fov, (m_CameraProjParams[1]) / (m_CameraProjParams[2]), m_Znear, m_Zfar);
	RecalculateViewMatrix();
	m_Type = CameraTypes::PERSPECTIVE;
}

void Doom::Camera::SetOrthographic(float ratio)
{
	this->m_Ratio = ratio;
	m_Znear = -5.0;
	m_Zfar = 100;
	m_CameraProjParams[0] = -ratio;
	m_CameraProjParams[1] = ratio;
	m_CameraProjParams[2] = 1;
	m_CameraProjParams[3] = -1;
	m_ProjectionMat4 = glm::ortho(
		m_CameraProjParams[0] * m_ZoomScale,
		m_CameraProjParams[1] * m_ZoomScale,
		m_CameraProjParams[3] * m_ZoomScale,
		m_CameraProjParams[2] * m_ZoomScale, m_Znear, m_Zfar);
	SetPosition(glm::vec3(m_Position.x, m_Position.y, 0.0f));
	m_Type = CameraTypes::ORTHOGRAPHIC;
}

glm::dvec3 Doom::Camera::GetMouseDirVec()
{
	glm::dvec2 pos;
	pos.x = ViewPort::GetInstance().GetStaticMousePosition().x / (m_Ratio * g_ScaleUI);
	pos.y = ViewPort::GetInstance().GetStaticMousePosition().y / (g_ScaleUI);
	glm::dvec4 clipCoords = glm::dvec4(pos.x, pos.y, -1.0f, 1.0f);
	glm::dvec4 eyeCoords = clipCoords * glm::inverse(m_ProjectionMat4);
	eyeCoords.z = -1.0f; eyeCoords.w = 0.0f;
	glm::dvec3 mouseRay = (glm::dvec3)(glm::inverse(m_ViewMat4) * eyeCoords);
	return Utils::Normalize(mouseRay);
}

void Doom::Camera::SetRotation(glm::vec3 rot)
{
	m_Pitch = rot.x; m_Yaw = rot.y; m_Roll = rot.z;
	RecalculateViewMatrix();
}

void Camera::Zoom(float zoomScale)
{
	this->m_ZoomScale = zoomScale;
	if (m_Type == CameraTypes::ORTHOGRAPHIC)
	{
		m_ProjectionMat4 = glm::ortho(
			m_CameraProjParams[0] * zoomScale,
			m_CameraProjParams[1] * zoomScale,
			m_CameraProjParams[3] * zoomScale,
			m_CameraProjParams[2] * zoomScale,
			m_Znear,
			m_Zfar);
		m_ViewProjectionMat4 = m_ProjectionMat4 * m_ViewMat4;
	}
}

void Camera::Increase() {
	if (Input::IsKeyDown(Keycode::KEY_LEFT_CONTROL))
	{
		if (Input::IsKeyDown(Keycode::KEY_PAGE_UP)) 
		{
			Zoom(abs(m_ZoomScale + (1 * DeltaTime::GetDeltaTime())));
		}
		else if (Input::IsKeyDown(Keycode::KEY_PAGE_DOWN)) 
		{
			if (m_ZoomScale <= 0) return;
			Zoom(abs(m_ZoomScale - (1 * DeltaTime::GetDeltaTime())));
		}
	}
}

void Camera::SetOnStart() {
	if (Input::IsKeyPressed(Keycode::KEY_F)) 
		SetPosition(glm::vec3(0, 0, 0));
}

void Camera::CameraMovement()
{
	if (ViewPort::GetInstance().m_IsActive && Input::IsMouseDown(Keycode::MOUSE_BUTTON_2)) 
	{
		float speed = 2.f;
		if (Input::IsKeyDown(Keycode::KEY_LEFT_SHIFT)) speed *= 10;
		if (m_Type == CameraTypes::ORTHOGRAPHIC)
		{
			if (Input::IsKeyDown(Keycode::KEY_W))
				MovePosition(glm::vec3(0, (speed * DeltaTime::GetDeltaTime() * m_ZoomScale), 0));
			if (Input::IsKeyDown(Keycode::KEY_S))
				MovePosition(glm::vec3(0, -(speed * DeltaTime::GetDeltaTime() * m_ZoomScale), 0));
			if (Input::IsKeyDown(Keycode::KEY_D))
				MovePosition(glm::vec3((speed * DeltaTime::GetDeltaTime() * m_ZoomScale), 0, 0));
			if (Input::IsKeyDown(Keycode::KEY_A))
				MovePosition(glm::vec3(-(speed * DeltaTime::GetDeltaTime() * m_ZoomScale), 0, 0));
			if (Input::IsKeyDown(Keycode::KEY_G))
			{
				if (Editor::GetInstance().go != nullptr)
					Editor::GetInstance().go->m_ComponentManager.GetComponent<Transform>()->Translate(
						ViewPort::GetInstance().GetMousePositionToWorldSpace().x,
						ViewPort::GetInstance().GetMousePositionToWorldSpace().y);
			}
			Increase();
		}
		else 
		{
			speed *= DeltaTime::s_Deltatime;
			if (Input::IsKeyDown(Keycode::KEY_W))
				MovePosition(glm::vec3(-m_Back.x, m_Back.y, -m_Back.z) * speed);
			if (Input::IsKeyDown(Keycode::KEY_S))
				MovePosition(glm::vec3(m_Back.x, -m_Back.y, m_Back.z) * speed);
			if (Input::IsKeyDown(Keycode::SPACE))
				MovePosition(glm::vec3(m_Up) * speed);
			if (Input::IsKeyDown(Keycode::KEY_C))
				MovePosition(glm::vec3(-m_Up) * speed);
		
			Window::GetInstance().DisableCursor();
			glm::dvec2 delta = ViewPort::GetInstance().GetMouseDragDelta();
			delta *= 0.2;
			glm::vec3 rot = Window::GetInstance().GetCamera().GetRotation();
			Window::GetInstance().GetCamera().SetRotation(glm::vec3((rot.x + delta.y * (2.0f * 3.14159f) / 360.0f), (rot.y - delta.x * (2.0f * 3.14159f) / 360.0f), 0.0f));
		
			if (Input::IsKeyDown(Keycode::KEY_D)) 
			{
				MovePosition(glm::vec3(m_Right) * speed);

			}
			if (Input::IsKeyDown(Keycode::KEY_A))
			{
				MovePosition(glm::vec3(-m_Right) * speed);
			}

			//Stupid, but interesting implementation of right vector
			/*glm::vec2 rightVec;
			rightVec = { -(m_Back.z * 1.0f) / (m_Back.x) , 1.0f };
			if (isinf(rightVec.x))
				rightVec = { 1,0 };
			rightVec = Utils::Normalize(rightVec);
			rightVec = m_Right;
			double angle = (m_Yaw * 360.0f) / (2 * 3.14159f);
			if (Input::IsKeyDown(Keycode::KEY_D))
			{
				if (angle > 0 && angle < 180)
					MovePosition(glm::vec3(-rightVec.x, 0, -rightVec.y) * speed);
				else if (angle >= 180)
					MovePosition(glm::vec3(rightVec.x, 0, rightVec.y) * speed);
				else if (angle <= -180)
					MovePosition(glm::vec3(-rightVec.x, 0, -rightVec.y) * speed);
				else
					MovePosition(glm::vec3(rightVec.x, 0, rightVec.y) * speed);
			}
			if (Input::IsKeyDown(Keycode::KEY_A))
			{
				if (angle > 0 && angle < 180)
					MovePosition(glm::vec3(rightVec.x, 0, rightVec.y) * speed);
				else if (angle >= 180)
					MovePosition(glm::vec3(-rightVec.x, 0, -rightVec.y) * speed);
				else if (angle <= -180)
					MovePosition(glm::vec3(rightVec.x, 0, rightVec.y) * speed);
				else
					MovePosition(glm::vec3(-rightVec.x, 0, -rightVec.y) * speed);
			}*/
		}
	}
	else 
	{
		Window::GetInstance().ShowCursor();
	}
	SetOnStart();
}