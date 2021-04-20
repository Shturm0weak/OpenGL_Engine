#pragma once
#include "../Core/Listener.h"
#include "../vendor/glm/glm.hpp"
#include "../Input/Input.h"
#include "../Enums/KeyCode.h"
#include "OpenGl/FrameBuffer.h"

namespace Doom {

	class DOOM_API Camera {
	public:

		enum CameraTypes {
			ORTHOGRAPHIC = 0,
			PERSPECTIVE = 1,
		};

		glm::dvec3 backV = { 0,0,-1 };
		float m_ZnearSM = -50.0f;
		float m_ZfarSM = 50.0f;
		float m_RationProjectionSM = 50.0f;
		CameraTypes m_Type = ORTHOGRAPHIC;

		Camera();
		~Camera();

		glm::vec3 GetRotation();
		glm::dvec3 GetMouseDirVec();
		glm::dvec3 GetForwardV();

		glm::vec3 GetPosition() const { return m_Position; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMat4; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMat4; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMat4; }
		float GetAspectRatio() const { return m_Ratio; }
		double GetZoomLevel() const { return m_ZoomLevel; }

		void WindowResize(glm::ivec2 size);
		void Increase();
		void SetOnStart();
		void CameraMovement();
		void RecalculateViewMatrix();
		void Zoom(float zoomlevel);
		void MovePosition(const glm::vec3 position);
		void SetPosition(const glm::vec3 position);
		void SetPerspective(float fovy, float width, float height, float znear, float zfar);
		void SetFov(float fov);
		void SetOrthographic(float ratio);
		void SetRotation(glm::vec3 rot);

		float GetRationWH() const;
		float GetRationHW() const;
		float GetFOV() { return m_Fov; }

		friend class Editor;
		friend class Batch;
		friend class Window;
		friend class ViewPort;
	private:

		std::mutex m_Mtx;
		glm::vec3 m_Position = glm::vec3(0);
		glm::mat4 m_ProjectionMat4;
		glm::mat4 m_ViewMat4;
		glm::mat4 m_ViewProjectionMat4;
		float m_Zfar = 10;
		float m_Znear = 0.1f;
		float m_Fov = 1;
		float m_Ratio = 0;
		float m_ZoomLevel = 1.0f;
		float m_Roll = 0.0f;
		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;
		float m_AspectRatio[4]; 
	};

}