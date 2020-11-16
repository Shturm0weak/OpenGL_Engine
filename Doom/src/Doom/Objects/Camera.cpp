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
	EventSystem::GetInstance()->RegisterClient(EventType::ONWINDOWRESIZE, this);
}

void Camera::RecalculateViewMatrix() {
	ThreadPool::GetInstance()->Enqueue([=] {
	std::lock_guard<std::mutex> lock(mtx);
	rot = glm::rotate(glm::mat4(1.0f), roll, glm::vec3(0, 0, 1))
					* glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0, 1, 0))
					* glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1, 0, 0));
	glm::mat4 transform = glm::translate(glm::mat4(1.0f),m_Position)
		* rot;

	float cosPitch = cos(pitch);

	forwardV.z = cos(yaw) * cosPitch;
	forwardV.x = sin(yaw) * cosPitch;
	forwardV.y = sin(pitch);

	m_ViewMatrix = glm::inverse(transform);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
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
	if (IsWindowResized) {
		if (props == nullptr)
			return;
		if (props[0] == 0 || props[1] == 0)
			return;
		glViewport(0, 0, props[0], props[1]);
		ratio = ViewPort::GetInstance()->GetSize().x / ViewPort::GetInstance()->GetSize().y;
		switch (type)
		{
		case Doom::Camera::ORTHOGRAPHIC:
			SetOrthographic(ratio);
			break;
		case Doom::Camera::PERSPECTIVE:
			SetPerspective(fovy, props[0], props[1], znear, zfar);
			break;
		default:
			break;
		}
		RecalculateViewMatrix();
		Gui::GetInstance()->RecalculateProjectionMatrix();
		//glBindTexture(GL_TEXTURE_2D, frameBuffer->texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, props[0], props[1], 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		//glBindTexture(GL_TEXTURE_2D, 0);
		IsWindowResized = false;
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
	this->ratio = width / height;
	aspectratio[0] = -width;
	aspectratio[1] = width;
	aspectratio[2] = height;
	aspectratio[3] = -height;
	this->zfar = (zfar);
	this->znear = (znear);
	this->fovy = (fovy);
	m_ProjectionMatrix = glm::perspective(fovy, width / height, znear, zfar);
	type = PERSPECTIVE;
}

void Doom::Camera::SetFov(float fov)
{
	m_ProjectionMatrix = glm::perspective(fovy, (aspectratio[1]) / (aspectratio[2]), znear, zfar);
	RecalculateViewMatrix();
	type = PERSPECTIVE;
}

void Doom::Camera::SetOrthographic(float ratio)
{
	this->ratio = ratio;
	znear = -1.0;
	zfar = 100;
	aspectratio[0] = -ratio;
	aspectratio[1] = ratio;
	aspectratio[2] = 1;
	aspectratio[3] = -1;
	m_ProjectionMatrix = glm::ortho(aspectratio[0] * zoomlevel, aspectratio[1] * zoomlevel, aspectratio[3] * zoomlevel, aspectratio[2] * zoomlevel, znear, zfar);
	type = ORTHOGRAPHIC;
}

glm::vec3 Doom::Camera::GetRotation()
{
	return glm::vec3(pitch, yaw, roll);
}

glm::vec3 Doom::Camera::GetMouseDirVec()
{
	glm::vec2 pos;
	pos.x = ViewPort::GetInstance()->GetStaticMousePosition().x / (Window::GetCamera().GetAspectRatio() * HEIGHT);
	pos.y = ViewPort::GetInstance()->GetStaticMousePosition().y / (HEIGHT);
	glm::vec4 clipCoords = glm::vec4(pos.x, pos.y, -1.0f, 1.0f);
	glm::vec4 eyeCoords = clipCoords * glm::inverse(m_ProjectionMatrix);
	eyeCoords.z = -1.0f; eyeCoords.w = 0.0f;
	glm::vec3 mouseRay = (glm::vec3)(glm::inverse(m_ViewMatrix) * eyeCoords);
	return glm::vec3(glm::normalize(mouseRay));
}

void Doom::Camera::SetRotation(glm::vec3 rot)
{
	pitch = rot.x; yaw = rot.y; roll = rot.z;
	RecalculateViewMatrix();
}

void Camera::Zoom(float zoomlevel)
{
	this->zoomlevel = zoomlevel;
	if(type == ORTHOGRAPHIC)
		m_ProjectionMatrix = glm::ortho(aspectratio[0] * zoomlevel,aspectratio[1] * zoomlevel,aspectratio[3] * zoomlevel,aspectratio[2] * zoomlevel,znear,zfar);
	else if(type == PERSPECTIVE)
		m_ProjectionMatrix = glm::perspective(fovy, abs(aspectratio[0]) / abs(aspectratio[3]),znear,zfar);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::OnWindowResize(void * _props)
{
	props = static_cast<int*>(_props);
	IsWindowResized = true;
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
	if (type == ORTHOGRAPHIC) {
		if (Input::IsKeyDown(Keycode::KEY_UP)) {
			MovePosition(glm::vec3(0, (20.f * DeltaTime::GetDeltaTime() * zoomlevel), 0));
		}
		if (Input::IsKeyDown(Keycode::KEY_DOWN)) {
			MovePosition(glm::vec3(0, -(20.f * DeltaTime::GetDeltaTime() * zoomlevel), 0));
		}
		if (Input::IsKeyDown(Keycode::KEY_RIGHT)) {
			MovePosition(glm::vec3((20.f * DeltaTime::GetDeltaTime() * zoomlevel), 0, 0));
		}
		if (Input::IsKeyDown(Keycode::KEY_LEFT)) {
			MovePosition(glm::vec3(-(20.f * DeltaTime::GetDeltaTime() * zoomlevel), 0, 0));
		}
		if (Input::IsKeyDown(Keycode::KEY_BACKSPACE)) {
			MovePosition(glm::vec3(0, 0, -(20.f * DeltaTime::GetDeltaTime() * zoomlevel)));
		}
		if (Input::IsKeyDown(Keycode::KEY_LEFT_SHIFT)) {
			MovePosition(glm::vec3(0, 0, (20.f * DeltaTime::GetDeltaTime() * zoomlevel)));
		}
		if (Input::IsKeyDown(Keycode::KEY_G)) {
			if (Editor::GetInstance()->go != nullptr) {
				Editor::GetInstance()->go->GetComponentManager()->GetComponent<Transform>()->Translate(ViewPort::GetInstance()->GetMousePositionToWorldSpace().x, ViewPort::GetInstance()->GetMousePositionToWorldSpace().y);
			}
		}
		Increase();
	}
	else {
		float speed = 2.f;
		if (Input::IsKeyDown(Keycode::KEY_LEFT_SHIFT)) {
			speed *= 10;
		}
		forwardV *= speed * DeltaTime::deltatime;
		if (Input::IsKeyDown(Keycode::KEY_W)) {
			MovePosition(glm::vec3(-forwardV.x, forwardV.y, -forwardV.z));
		}
		if (Input::IsKeyDown(Keycode::KEY_S)) {
			MovePosition(glm::vec3(forwardV.x, -forwardV.y, forwardV.z));
		}
		if (Input::IsKeyDown(Keycode::SPACE)) {
			MovePosition(glm::vec3(0, 5.0f * DeltaTime::deltatime, 0));
		}
		if (Input::IsKeyDown(Keycode::KEY_C)) {
			MovePosition(glm::vec3(0, -5.0f * DeltaTime::deltatime, 0));
		}
		if (ViewPort::GetInstance()->IsActive && Input::IsMouseDown(Keycode::MOUSE_BUTTON_2)) {
			glfwSetInputMode(Window::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glm::dvec2 delta = ViewPort::GetInstance()->GetMouseDragDelta();
			delta *= 0.2;
			SetRotation(glm::vec3((pitch + delta.y * (2 * 3.14159f) / 360.0f), (yaw - delta.x * (2 * 3.14159f) / 360.0f), 0));
			if (yaw > glm::two_pi<float>() || yaw < -glm::two_pi<float>())
				yaw = 0;
			if (pitch > glm::two_pi<float>() || pitch < -glm::two_pi<float>())
				pitch = 0;
		}
		else {
			glfwSetInputMode(Window::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		glm::vec2 rightVec;
		rightVec = { -(forwardV.z * 1) / (forwardV.x) ,1 };
		if (isinf(rightVec.x))
			rightVec = {1,0};
		rightVec *= (1.f / sqrt(rightVec.x * rightVec.x + rightVec.y * rightVec.y));
		rightVec *= DeltaTime::deltatime * speed;
		double angle = (yaw * 360.0f) / (2 * 3.14159f);
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
	SetOnStart();
}

float Camera::GetRationWH() const
{ return ((float)ViewPort::GetInstance()->GetSize()[0] / (float)ViewPort::GetInstance()->GetSize()[1]); }
float Camera::GetRationHW() const
{ return ((float)ViewPort::GetInstance()->GetSize()[1] / (float)ViewPort::GetInstance()->GetSize()[0]); }

