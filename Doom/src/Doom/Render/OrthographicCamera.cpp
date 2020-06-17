#include "../pch.h"
#include "OrthographicCamera.h"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/glm/gtc/type_ptr.hpp"
#include "../vendor/glm/gtx/rotate_vector.hpp"
#include "../Core/Editor.h"
#include "ViewPort.h"

using namespace Doom;

Camera::Camera(float left,float right,float top,float bottom,float znear,float zfar)
	: znear(znear),zfar(zfar),m_ProjectionMatrix(glm::ortho(left * zoomlevel,right * zoomlevel,bottom * zoomlevel,top * zoomlevel,znear,zfar)),m_ViewMatrix(1.0f)
{
	EventSystem::Instance()->RegisterClient("OnWindowResize", this);
	aspectratio[0] = left;
	aspectratio[1] = right;
	aspectratio[2] = top;
	aspectratio[3] = bottom;
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	
}

void Camera::RecalculateViewMatrix() {
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), roll, glm::vec3(0, 0, 1))
					* glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0, 1, 0))
					* glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1, 0, 0));
	glm::mat4 transform = glm::translate(glm::mat4(1.0f),m_Position)
		* rot;

	startDir.z = cos(yaw)*cos(pitch);
	startDir.x = sin(yaw)*cos(pitch);
	startDir.y = sin(pitch);

	m_ViewMatrix = glm::inverse(transform);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::WindowResize() {
	if (props == nullptr)
		return;
	if (IsWindowResized) {
		if (props[0] == 0 || props[1] == 0)
			return;
		glViewport(0, 0, props[0], props[1]);
		//glBindTexture(GL_TEXTURE_2D, frameBuffer->texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, props[0], props[1], 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		//glBindTexture(GL_TEXTURE_2D, 0);
		IsWindowResized = false;
	}
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

void Doom::Camera::ChangeAspectRatio(float width, float height)
{
	aspectratio[0] = -width;
	aspectratio[1] = width;
	aspectratio[2] = height;
	aspectratio[3] = -height;
	if (type == ORTHOGRAPHIC)
		m_ProjectionMatrix = glm::ortho(aspectratio[0] * zoomlevel, aspectratio[1] * zoomlevel, aspectratio[3] * zoomlevel, aspectratio[2] * zoomlevel, -1.0f, 1.0f);
	else if (type == PERSPECTIVE)
		m_ProjectionMatrix = glm::perspective(fovy, abs(aspectratio[0]) / abs(aspectratio[3]), znear, zfar);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
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
			if (Editor::Instance()->go != nullptr) {
				Editor::Instance()->go->GetComponentManager()->GetComponent<Transform>()->Translate(ViewPort::Instance()->GetMousePositionToWorldSpace().x, ViewPort::Instance()->GetMousePositionToWorldSpace().y);
			}
		}
		Increase();
	}
	else {
		float speed = 2.f;
		if (Input::IsKeyDown(Keycode::KEY_LEFT_SHIFT)) {
			speed *= 3;

		}
		startDir *= speed * DeltaTime::deltatime;
		if (Input::IsKeyDown(Keycode::KEY_W)) {
			MovePosition(glm::vec3(-startDir.x, startDir.y, -startDir.z));
		}
		if (Input::IsKeyDown(Keycode::KEY_S)) {
			MovePosition(glm::vec3(startDir.x, -startDir.y, startDir.z));

		}
		if (Input::IsKeyDown(Keycode::SPACE)) {
			MovePosition(glm::vec3(0, 5.0f * DeltaTime::deltatime, 0));
		}
		if (Input::IsKeyDown(Keycode::KEY_C)) {
			MovePosition(glm::vec3(0, -5.0f * DeltaTime::deltatime, 0));
		}
		if (ViewPort::Instance()->IsActive) {
			glm::vec2 drag = ViewPort::Instance()->GetMousePositionToScreenSpace();
			drag *= 0.2;
			SetRotation(glm::vec3((drag.y * (2 * 3.14159f) / 360.0f), (-drag.x * (2 * 3.14159f) / 360.0f), 0));
		}
		glm::vec2 rightVec = { -(startDir.z * 1) / startDir.x ,1 };
		rightVec *= (1.f / sqrtf(rightVec.x * rightVec.x + rightVec.y * rightVec.y));
		rightVec *= DeltaTime::deltatime * speed;
		double angle = (yaw * 360.0f) / (2 * 3.14159f);
		if (Input::IsKeyDown(Keycode::KEY_D)) {
			if (angle >= 0 && angle < 180)
				MovePosition(glm::vec3(-rightVec.x, 0, -rightVec.y));
			else if (angle >= 180)
				MovePosition(glm::vec3(rightVec.x, 0, rightVec.y));
			else if (angle <= -180)
				MovePosition(glm::vec3(-rightVec.x, 0, -rightVec.y));
			else
				MovePosition(glm::vec3(rightVec.x, 0, rightVec.y));

		}
		if (Input::IsKeyDown(Keycode::KEY_A)) {
			if (angle >= 0 && angle < 180)
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

glm::vec2 Doom::Camera::GetRatio()
{ 
	return glm::vec2(GetAspectRatio().x / (float)Window::GetProps()[0], GetAspectRatio().y / (float)Window::GetProps()[1]);
}

float Camera::GetRationWH()
{ return ((float)Window::GetProps()[0] / (float)Window::GetProps()[1]); }
float Camera::GetRationHW()
{ return ((float)Window::GetProps()[1] / (float)Window::GetProps()[0]); }

