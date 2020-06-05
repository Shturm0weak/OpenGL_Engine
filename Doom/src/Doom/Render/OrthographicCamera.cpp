#include "../pch.h"
#include "OrthographicCamera.h"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../Core/Editor.h"
#include "ViewPort.h"

using namespace Doom;

OrthographicCamera::OrthographicCamera(float left,float right,float top,float bottom)
	: m_ProjectionMatrix(glm::ortho(left * zoomlevel,right * zoomlevel,bottom * zoomlevel,top * zoomlevel,-1.f,1.f)),m_ViewMatrix(1.0f)
{
	EventSystem::Instance()->RegisterClient("OnWindowResize", this);
	aspectration[0] = left;
	aspectration[1] = right;
	aspectration[2] = top;
	aspectration[3] = bottom;
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	
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
		//glBindTexture(GL_TEXTURE_2D, frameBuffer->texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, props[0], props[1], 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		//glBindTexture(GL_TEXTURE_2D, 0);
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

void Doom::OrthographicCamera::ChangeAspectRation(float left, float right, float top, float bottom)
{
	aspectration[0] = left;
	aspectration[1] = right;
	aspectration[2] = top;
	aspectration[3] = bottom;
	m_ProjectionMatrix = glm::ortho(aspectration[0] * zoomlevel, aspectration[1] * zoomlevel, aspectration[3] * zoomlevel, aspectration[2] * zoomlevel, -1.0f, 1.0f);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::Increase() {
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

void OrthographicCamera::SetOnStart() {
	if (Input::IsKeyPressed(Keycode::KEY_F)) {
		SetPosition(glm::vec3(0, 0, 0));
	}
}

void OrthographicCamera::CameraMovement() {
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
	if (Input::IsKeyDown(Keycode::KEY_G)) {
		if (Editor::Instance()->selectedGO != nullptr) {
			Editor::Instance()->selectedGO->GetComponentManager()->GetComponent<Transform>()->Translate(ViewPort::Instance()->GetMousePositionToWorldSpace().x, ViewPort::Instance()->GetMousePositionToWorldSpace().y);
		}
	}
	Increase();
	SetOnStart();
}

glm::vec2 Doom::OrthographicCamera::GetRatio()
{ 
	return glm::vec2(GetAspectRation().x / Window::GetProps()[0], GetAspectRation().y / Window::GetProps()[1]);
}


