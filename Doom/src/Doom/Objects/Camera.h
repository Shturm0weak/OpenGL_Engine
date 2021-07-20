#pragma once

#include "../Core/Listener.h"
#include "../vendor/glm/glm.hpp"
#include "../Input/Input.h"
#include "../Enums/KeyCode.h"
#include "OpenGl/FrameBuffer.h"

namespace Doom {

	class DOOM_API Camera {
	public:

		enum class CameraTypes {
			ORTHOGRAPHIC = 0,
			PERSPECTIVE = 1,
		};

		glm::dvec3 m_Back;
		glm::dvec3 m_Forward;
		glm::dvec3 m_Right;
		glm::dvec3 m_Up;
		CameraTypes m_Type = CameraTypes::ORTHOGRAPHIC;

		Camera();
		~Camera();

		glm::vec3 GetRotation() { return glm::vec3(m_Pitch, m_Yaw, m_Roll); }
		glm::vec3 GetPosition() { return m_Position; }
		glm::dvec3 GetMouseDirVec();

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMat4; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMat4; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMat4; }

		void Increase();
		void SetOnStart();
		void CameraMovement();
		void RecalculateViewMatrix();
		void Zoom(float zoomScale);
		void MovePosition(const glm::vec3& position);
		void SetPosition(const glm::vec3& position);
		void SetPerspective(float fov, float width, float height, float znear, float zfar);
		void SetFov(float fov);
		void SetOrthographic(float ratio);
		void SetRotation(glm::vec3 rot);

		glm::vec3 m_Position = glm::vec3(0);
		float m_ZoomScale = 1.0f;
		float m_Fov = glm::radians(90.0f);
		float m_Ratio = 0;
	private:

		glm::mat4 m_ProjectionMat4;
		glm::mat4 m_ViewMat4;
		glm::mat4 m_ViewProjectionMat4;
		float m_Zfar = 10;
		float m_Znear = 0.1f;
		float m_Roll = 0.0f;
		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;
		float m_CameraProjParams[4]; 

		friend class Editor;
		friend class Batch;
		friend class Window;
		friend class ViewPort;
	};

}