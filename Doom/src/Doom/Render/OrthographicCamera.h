#pragma once
#include "../Core/Listener.h"
#include "../vendor/glm/glm.hpp"
#include "../Core/Input.h"
#include "../Core/KeyCode.h"

namespace Doom {

	class DOOM_API OrthographicCamera : public Listener {
	public:
		OrthographicCamera(float left, float right, float top, float bottom);

		const glm::vec3 GetPosition() const { return glm::vec3(m_Position.x, m_Position.y, m_Position.z); }
		void MovePosition(const glm::vec3 position) {
			m_Position.x += position.x;
			m_Position.y += position.y;
			m_Position.z += position.z;

			RecalculateViewMatrix();
			//std::cout << "x " << m_Position.x << " y " << m_Position.y << " z " << m_Position.z << std::endl;
		}

		void WindowResize();

		void SetPosition(const glm::vec3 position) {
			m_Position.x = position.x;
			m_Position.y = position.y;
			m_Position.z = position.z;

			RecalculateViewMatrix();
			//std::cout << "x " << m_Position.x << " y " << m_Position.y << " z " << m_Position.z << std::endl;
		}

		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		void Increase();

		void SetOnStart();

		void CameraMovement();


		void RecalculateViewMatrix();
		void Zoom(float zoomlevel);

		float GetZoomLevel() { return zoomlevel; }

		friend class Editor;
	private:
		std::string Name = "Particle System";
		bool IsWindowResized = false;
		int* props = nullptr;
		virtual void OnWindowResize(void* props)override;
		float zoomlevel = 1.0f;
		glm::vec3 m_Position;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;
		float m_Rotation = 0.0f;
		float aspectration[4];

	};

}