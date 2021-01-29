#include "../pch.h"
#include "Camera.h"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/glm/gtc/type_ptr.hpp"
#include "../vendor/glm/gtx/rotate_vector.hpp"
#include "../Core/Editor.h"
#include "../Core/ViewPort.h"
#include "../Text/Gui.h"

using namespace Doom;

Camera::Camera(){
	m_Position = glm::vec3(0, 0, 0);
	EventSystem::GetInstance().RegisterClient(EventType::ONWINDOWRESIZE, this);
}

Doom::Camera::~Camera()
{
	delete m_FrameBufferBlur[0];
	delete m_FrameBufferBlur[1];
	delete m_FrameBufferColor;
	delete m_FrameBufferShadowMap;
}

void Camera::RecalculateViewMatrix() {
	ThreadPool::GetInstance().Enqueue([=] {
	std::lock_guard<std::mutex> lock(m_Mtx);
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), m_Roll, glm::vec3(0, 0, 1))
					* glm::rotate(glm::mat4(1.0f), m_Yaw, glm::vec3(0, 1, 0))
					* glm::rotate(glm::mat4(1.0f), m_Pitch, glm::vec3(1, 0, 0));
	glm::mat4 transform = glm::translate(glm::mat4(1.0f),m_Position)
		* rot;

	float cosPitch = cos(m_Pitch);

	backV.z = cos(m_Yaw) * cosPitch;
	backV.x = sin(m_Yaw) * cosPitch;
	backV.y = sin(m_Pitch);

	m_ViewMat4 = glm::inverse(transform);
	m_ViewProjectionMat4 = m_ProjectionMat4 * m_ViewMat4;
	});
}

void Doom::Camera::MovePosition(const glm::vec3 position)
{
	m_Position.x += position.x;
	m_Position.y += position.y;
	m_Position.z += position.z;

	RecalculateViewMatrix();
	
	//std::cout << "x " << m_Position.x << " y " << m_Position.y << " z " << m_Position.z << std::endl;
}

void Camera::WindowResize() {
	if (m_IsWindowResized) {
		if (m_Props == nullptr)
			return;
		if (m_Props[0] == 0 || m_Props[1] == 0)
			return;
		glViewport(0, 0, m_Props[0], m_Props[1]);
		m_Ratio = ViewPort::GetInstance()->GetSize().x / ViewPort::GetInstance()->GetSize().y;
		switch (m_Type)
		{
		case Doom::Camera::ORTHOGRAPHIC:
			SetOrthographic(m_Ratio);
			break;
		case Doom::Camera::PERSPECTIVE:
			SetPerspective(m_Fov, ViewPort::GetInstance()->GetSize().x, ViewPort::GetInstance()->GetSize().y, m_Znear, m_Zfar);
			break;
		default:
			break;
		}
		RecalculateViewMatrix();
		Gui::GetInstance().RecalculateProjectionMatrix();
		//glBindTexture(GL_TEXTURE_2D, frameBuffer->texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, props[0], props[1], 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		//glBindTexture(GL_TEXTURE_2D, 0);
		m_IsWindowResized = false;
	}
}

void Doom::Camera::SetPosition(const glm::vec3 position)
{
	m_Position.x = position.x;
	m_Position.y = position.y;
	m_Position.z = position.z;

	RecalculateViewMatrix();
	//std::cout << "x " << m_Position.x << " y " << m_Position.y << " z " << m_Position.z << std::endl;
}

void Doom::Camera::SetPerspective(float fovy, float width, float height, float znear, float zfar)
{
	this->m_Ratio = width / height;
	m_AspectRatio[0] = -width;
	m_AspectRatio[1] = width;
	m_AspectRatio[2] = height;
	m_AspectRatio[3] = -height;
	this->m_Zfar = (zfar);
	this->m_Znear = (znear);
	this->m_Fov = (fovy);
	m_ProjectionMat4 = glm::perspective(fovy, width / height, znear, zfar);
	m_Type = PERSPECTIVE;
}

void Doom::Camera::SetFov(float fov)
{
	m_ProjectionMat4 = glm::perspective(m_Fov, (m_AspectRatio[1]) / (m_AspectRatio[2]), m_Znear, m_Zfar);
	RecalculateViewMatrix();
	m_Type = PERSPECTIVE;
}

void Doom::Camera::SetOrthographic(float ratio)
{
	this->m_Ratio = ratio;
	m_Znear = -1.0;
	m_Zfar = 100;
	m_AspectRatio[0] = -ratio;
	m_AspectRatio[1] = ratio;
	m_AspectRatio[2] = 1;
	m_AspectRatio[3] = -1;
	m_ProjectionMat4 = glm::ortho(m_AspectRatio[0] * m_ZoomLevel, m_AspectRatio[1] * m_ZoomLevel, m_AspectRatio[3] * m_ZoomLevel, m_AspectRatio[2] * m_ZoomLevel, m_Znear, m_Zfar);
	m_Type = ORTHOGRAPHIC;
}

glm::vec3 Doom::Camera::GetRotation()
{
	return glm::vec3(m_Pitch, m_Yaw, m_Roll);
}

glm::vec3 Doom::Camera::GetMouseDirVec()
{
	glm::vec2 pos;
	pos.x = ViewPort::GetInstance()->GetStaticMousePosition().x / (Window::GetInstance().GetCamera().GetAspectRatio() * g_Height);
	pos.y = ViewPort::GetInstance()->GetStaticMousePosition().y / (g_Height);
	glm::vec4 clipCoords = glm::vec4(pos.x, pos.y, -1.0f, 1.0f);
	glm::vec4 eyeCoords = clipCoords * glm::inverse(m_ProjectionMat4);
	eyeCoords.z = -1.0f; eyeCoords.w = 0.0f;
	glm::vec3 mouseRay = (glm::vec3)(glm::inverse(m_ViewMat4) * eyeCoords);
	return glm::vec3(glm::normalize(mouseRay));
}

glm::vec3 Doom::Camera::GetForwardV()
{
	return glm::vec3(-backV.x, backV.y, -backV.z);
}

void Doom::Camera::SetRotation(glm::vec3 rot)
{
	m_Pitch = rot.x; m_Yaw = rot.y; m_Roll = rot.z;
	RecalculateViewMatrix();
}

void Camera::Zoom(float zoomlevel)
{
	this->m_ZoomLevel = zoomlevel;
	if(m_Type == ORTHOGRAPHIC)
		m_ProjectionMat4 = glm::ortho(m_AspectRatio[0] * zoomlevel,m_AspectRatio[1] * zoomlevel,m_AspectRatio[3] * zoomlevel,m_AspectRatio[2] * zoomlevel,m_Znear,m_Zfar);
	else if(m_Type == PERSPECTIVE)
		m_ProjectionMat4 = glm::perspective(m_Fov, abs(m_AspectRatio[0]) / abs(m_AspectRatio[3]),m_Znear,m_Zfar);
	m_ViewProjectionMat4 = m_ProjectionMat4 * m_ViewMat4;
}

void Camera::OnWindowResize(void * _props)
{
	m_Props = static_cast<int*>(_props);
	m_IsWindowResized = true;
}

void Camera::Increase() {
	if (Input::IsKeyDown(Keycode::KEY_LEFT_CONTROL)) {
		if (Input::IsKeyDown(Keycode::KEY_PAGE_UP)) {
			Zoom(abs(GetZoomLevel() + (1 * DeltaTime::GetDeltaTime())));
		}
		else if (Input::IsKeyDown(Keycode::KEY_PAGE_DOWN)) {
			if (GetZoomLevel() <= 0)
				return;
			Zoom(abs(GetZoomLevel() - (1 * DeltaTime::GetDeltaTime())));
		}
	}
}

void Camera::SetOnStart() {
	if (Input::IsKeyPressed(Keycode::KEY_F)) {
		SetPosition(glm::vec3(0, 0, 0));
	}
}

void Camera::CameraMovement() {
	if (ViewPort::GetInstance()->m_IsActive && Input::IsMouseDown(Keycode::MOUSE_BUTTON_2)) {
	if (m_Type == ORTHOGRAPHIC) {
		if (Input::IsKeyDown(Keycode::KEY_UP)) {
			MovePosition(glm::vec3(0, (20.f * DeltaTime::GetDeltaTime() * m_ZoomLevel), 0));
		}
		if (Input::IsKeyDown(Keycode::KEY_DOWN)) {
			MovePosition(glm::vec3(0, -(20.f * DeltaTime::GetDeltaTime() * m_ZoomLevel), 0));
		}
		if (Input::IsKeyDown(Keycode::KEY_RIGHT)) {
			MovePosition(glm::vec3((20.f * DeltaTime::GetDeltaTime() * m_ZoomLevel), 0, 0));
		}
		if (Input::IsKeyDown(Keycode::KEY_LEFT)) {
			MovePosition(glm::vec3(-(20.f * DeltaTime::GetDeltaTime() * m_ZoomLevel), 0, 0));
		}
		if (Input::IsKeyDown(Keycode::KEY_BACKSPACE)) {
			MovePosition(glm::vec3(0, 0, -(20.f * DeltaTime::GetDeltaTime() * m_ZoomLevel)));
		}
		if (Input::IsKeyDown(Keycode::KEY_LEFT_SHIFT)) {
			MovePosition(glm::vec3(0, 0, (20.f * DeltaTime::GetDeltaTime() * m_ZoomLevel)));
		}
		if (Input::IsKeyDown(Keycode::KEY_G)) {
			if (Editor::GetInstance().go != nullptr) {
				Editor::GetInstance().go->GetComponentManager()->GetComponent<Transform>()->Translate(ViewPort::GetInstance()->GetMousePositionToWorldSpace().x, ViewPort::GetInstance()->GetMousePositionToWorldSpace().y);
			}
		}
		Increase();
	}
	else {
		float speed = 2.f;
		if (Input::IsKeyDown(Keycode::KEY_LEFT_SHIFT)) {
			speed *= 10;
		}
		backV *= speed * DeltaTime::s_Deltatime;
		if (Input::IsKeyDown(Keycode::KEY_W)) {
			MovePosition(glm::vec3(-backV.x, backV.y, -backV.z));
		}
		if (Input::IsKeyDown(Keycode::KEY_S)) {
			MovePosition(glm::vec3(backV.x, -backV.y, backV.z));
		}
		if (Input::IsKeyDown(Keycode::SPACE)) {
			MovePosition(glm::vec3(0, 5.0f * DeltaTime::s_Deltatime, 0));
		}
		if (Input::IsKeyDown(Keycode::KEY_C)) {
			MovePosition(glm::vec3(0, -5.0f * DeltaTime::s_Deltatime, 0));
		}
		
		Window::GetInstance().DisableCursor();
		glm::dvec2 delta = ViewPort::GetInstance()->GetMouseDragDelta();
		delta *= 0.2;
		glm::vec3 rot = Window::GetInstance().GetCamera().GetRotation();
		Window::GetInstance().GetCamera().SetRotation(glm::vec3((rot.x + delta.y * (2 * 3.14159f) / 360.0f), (rot.y - delta.x * (2 * 3.14159f) / 360.0f), 0));
		
		glm::vec2 rightVec;
		rightVec = { -(backV.z * 1) / (backV.x) ,1 };
		if (isinf(rightVec.x))
			rightVec = {1,0};
		rightVec *= (1.f / sqrt(rightVec.x * rightVec.x + rightVec.y * rightVec.y));
		rightVec *= DeltaTime::s_Deltatime * speed;
		double angle = (m_Yaw * 360.0f) / (2 * 3.14159f);
		if (Input::IsKeyDown(Keycode::KEY_D)) {
			if (angle > 0 && angle < 180)
				MovePosition(glm::vec3(-rightVec.x, 0, -rightVec.y));
			else if (angle >= 180)
				MovePosition(glm::vec3(rightVec.x, 0, rightVec.y));
			else if (angle <= -180)
				MovePosition(glm::vec3(-rightVec.x, 0, -rightVec.y));
			else
				MovePosition(glm::vec3(rightVec.x, 0, rightVec.y));

		}
		if (Input::IsKeyDown(Keycode::KEY_A)) {
			if (angle > 0 && angle < 180)
				MovePosition(glm::vec3(rightVec.x, 0, rightVec.y));
			else if (angle >= 180)
				MovePosition(glm::vec3(-rightVec.x, 0, -rightVec.y));
			else if (angle <= -180)
				MovePosition(glm::vec3(rightVec.x, 0, rightVec.y));
			else
				MovePosition(glm::vec3(-rightVec.x, 0, -rightVec.y));
		}
	}
	}
	else {
		Window::GetInstance().ShowCursor();
	}
	SetOnStart();
}

float Camera::GetRationWH() const
{ return ((float)ViewPort::GetInstance()->GetSize()[0] / (float)ViewPort::GetInstance()->GetSize()[1]); }
float Camera::GetRationHW() const
{ return ((float)ViewPort::GetInstance()->GetSize()[1] / (float)ViewPort::GetInstance()->GetSize()[0]); }

