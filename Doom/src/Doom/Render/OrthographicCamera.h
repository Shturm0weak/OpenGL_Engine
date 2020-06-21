#pragma once
#include "../Core/Listener.h"
#include "../vendor/glm/glm.hpp"
#include "../Core/Input.h"
#include "../Core/KeyCode.h"
#include "FrameBuffer.h"

namespace Doom {

	class DOOM_API Camera : public Listener {

	public:
		enum CameraTypes {
			ORTHOGRAPHIC = 0,
			PERSPECTIVE = 1,
		};

		CameraTypes type = ORTHOGRAPHIC;

		FrameBuffer* frameBuffer = nullptr;

		Camera(float left, float right, float top, float bottom,float znear,float zfar);

		glm::vec3 GetPosition() const { return glm::vec3(m_Position.x, m_Position.y, m_Position.z); }
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

		void SetPerspective(float fovy,float width,float height,float znear,float zfar){
			aspectratio[0] = -width;
			aspectratio[1] = width;
			aspectratio[2] = height;
			aspectratio[3] = -height;
			this->zfar = (zfar);
			this->znear = (znear);
			this->fovy = (fovy);
			m_ProjectionMatrix = glm::perspective(fovy,width/height,znear,zfar);
			type = PERSPECTIVE;
		}

		void SetFov(float fov) {
			m_ProjectionMatrix = glm::perspective(fovy, (aspectratio[1]) / (aspectratio[2]), znear, zfar);
			RecalculateViewMatrix();
			type = PERSPECTIVE;
		}

		void SetOrthographic(float left, float right, float top, float bottom,float znear,float zfar) {
			this->znear = znear;
			this->zfar = zfar;
			aspectratio[0] = left;
			aspectratio[1] = right;
			aspectratio[2] = top;
			aspectratio[3] = bottom;
			m_ProjectionMatrix = glm::ortho(left * zoomlevel, right * zoomlevel, bottom * zoomlevel, top * zoomlevel, znear,zfar);
			type = ORTHOGRAPHIC;
		}

		glm::vec3 GetRotation() const { return glm::vec3(pitch, yaw, roll); }
		void SetRotation(glm::vec3 rot) { 
			pitch = rot.x; yaw = rot.y; roll = rot.z;
			RecalculateViewMatrix();
		}

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		void ChangeAspectRatio(float width,float height);

		void Increase();

		void SetOnStart();

		void CameraMovement();

		glm::vec2 GetRatio();
		float GetRationWH();
		float GetRationHW();
		glm::vec2 GetAspectRatio() { return glm::vec2(abs(aspectratio[0]),abs(aspectratio[3])); }
		void RecalculateViewMatrix();
		void Zoom(float zoomlevel);

		double GetZoomLevel() { return zoomlevel; }

		friend class Editor;
	private:
		//std::string Name = "Particle System";
		float zfar = 10;
		float znear = 0.1f;
		float fovy = 1;
		bool IsWindowResized = false;
		int* props = nullptr;
		virtual void OnWindowResize(void* props)override;
		float zoomlevel = 1.0f;
		glm::vec3 m_Position;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float aspectratio[4];
		public:
			glm::vec3 startDir = { 0,0,-1 };

	};

}