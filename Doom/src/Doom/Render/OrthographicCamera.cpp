#include "../pch.h"
#include "OrthographicCamera.h"
#include "../vendor/glm/gtc/matrix_transform.hpp"

using namespace Doom;

OrthographicCamera::OrthographicCamera(float left,float right,float top,float bottom)
	: m_ProjectionMatrix(glm::ortho(left * zoomlevel,right * zoomlevel,bottom * zoomlevel,top * zoomlevel,-1.f,1.f)),m_ViewMatrix(1.0f)
{
	EventSystem::Instance()->RegisterClient("OnWindowResize", this);
	aspectration[0] = left;
	aspectration[1] = right;
	aspectration[2] = top;
	aspectration[3] = bottom;
}

void OrthographicCamera::RecalculateViewMatrix() {
	glm::mat4 transform = glm::translate(glm::mat4(1.0f),m_Position)
		* glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0, 0, 1));

	m_ViewMatrix = glm::inverse(transform);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::WindowResize() {
	if (props == nullptr)
		return;
	if (IsWindowResized) {
		if (props[0] == 0 || props[1] == 0)
			return;
		glViewport(0, 0, props[0], props[1]);
		IsWindowResized = false;
	}
}

void OrthographicCamera::Zoom(float zoomlevel)
{
	this->zoomlevel = zoomlevel;
	m_ProjectionMatrix = glm::ortho(aspectration[0] * zoomlevel,aspectration[1] * zoomlevel,aspectration[3] * zoomlevel,aspectration[2] * zoomlevel,-1.0f,1.0f);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::OnWindowResize(void * _props)
{
	props = static_cast<int*>(_props);
	IsWindowResized = true;
}

void OrthographicCamera::Increase() {
	if (Input::IsKeyPressed(Keycode::KEY_LEFT_CONTROL)) {
		if (Input::IsKeyPressed(Keycode::KEY_PAGE_UP)) {
			Zoom(abs(GetZoomLevel() + (1 * DeltaTime::GetDeltaTime())));
		}
		else if (Input::IsKeyPressed(Keycode::KEY_PAGE_DOWN)) {
			if (GetZoomLevel() <= 0)
				return;
			Zoom(abs(GetZoomLevel() - (1 * DeltaTime::GetDeltaTime())));
		}
	}
}

void OrthographicCamera::SetOnStart() {
	if (Input::IsKeyPressed(Keycode::KEY_F)) {
		SetPosition(glm::vec3(0, 0, 0));
	}
}

void OrthographicCamera::CameraMovement() {
	if (Input::IsKeyPressed(Keycode::KEY_UP)) {
		MovePosition(glm::vec3(0, (20.f * DeltaTime::GetDeltaTime()), 0));
	}
	if (Input::IsKeyPressed(Keycode::KEY_DOWN)) {
		MovePosition(glm::vec3(0, -(20.f * DeltaTime::GetDeltaTime()), 0));
	}
	if (Input::IsKeyPressed(Keycode::KEY_RIGHT)) {
		MovePosition(glm::vec3((20.f * DeltaTime::GetDeltaTime()), 0, 0));
	}
	if (Input::IsKeyPressed(Keycode::KEY_LEFT)) {
		MovePosition(glm::vec3(-(20.f * DeltaTime::GetDeltaTime()), 0, 0));
	}
	Increase();
	SetOnStart();
}


