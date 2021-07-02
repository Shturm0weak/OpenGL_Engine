#pragma once

#include "../ECS/Component.h"

namespace Doom {

	class Irenderer;
	class RectangleCollider2D;

	class DOOM_API Transform : public Component {
	private:

		glm::mat4 m_ScaleMat4 = glm::mat4(1.f);
		glm::mat4 m_PosMat4 = glm::mat4(1.f);
		glm::mat4 m_ViewMat4 = glm::mat4(1.f);
		glm::vec3 m_Rotation = glm::vec3(0.0);
		glm::vec3 m_PrevPosition;

		void Copy(const Transform& rhs);

		friend class ComponentManager;
		friend class RectangleCollider2D;
		friend class DirectionalLight;
		friend class Renderer3D;
		friend class SpriteRenderer;
		friend class Instancing;
		friend class Batch;
		friend class Editor;
		friend class CubeCollider3D;
		friend class Renderer;
		friend class Ray3D;
		friend class GameObject;
		friend class SphereCollider;
		friend class World;
		friend class ViewPort;
	public:

		glm::mat4 GetTransform();
		glm::vec3 GetRotation();
		glm::vec3 GetPosition();
		glm::vec3 GetScale();

		Transform(const Transform& rhs);
		Transform(){}
		Transform(GameObject* owner);
		~Transform() {}

		void operator=(const Transform& rhs);

		static Component* Create();

		void Move(glm::vec3 dir = glm::vec3(0.0f));
		void Rotate(glm::vec3 angles = glm::vec3(0.0f), bool isRad = false);
		void Scale(glm::vec3 scale = glm::vec3(1.0f));
		void Translate(glm::vec3 pos = glm::vec3(0.0f));
		void RotateOnce(glm::vec3 angles = glm::vec3(0.0f), bool isRad = false);
		void RotateOnce(glm::vec3 direction, glm::vec3 axis);

		void Move(float x = 0.0f, float y = 0.0f, float z = 0.0f);
		void Rotate(float x = 0.0f, float y = 0.0f, float z = 0.0f, bool isRad = false);
		void Scale(float x = 0.0f, float y = 0.0f, float z = 0.0f);
		void Translate(float x = 0.0f, float y = 0.0f, float z = 0.0f);
		void RotateOnce(float x = 0.0f, float y = 0.0f, float z = 0.0f, bool isRad = false);


	};

}