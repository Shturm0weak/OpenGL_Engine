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

		//static std::map<char*, uint64_t> s_MemoryPool;
		//static std::vector<char*> s_FreeMemory;

		//char* m_MemoryPoolPtr = nullptr;

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
		friend class Gizmos;
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
		virtual void Delete() override;

		void Move(glm::vec3 dir);
		void Rotate(glm::vec3 angles, bool isRad = false);
		void Scale(glm::vec3 scale);
		void Translate(glm::vec3 pos);
		void RotateOnce(glm::vec3 angles, bool isRad = false);

		void Move(float x = 0, float y = 0, float z = 0);
		void Rotate(float x, float y, float z, bool isRad = false);
		void Scale(float x, float y,float z = 0);
		void Translate(float x = 0, float y = 0,float z = 0);
		void RotateOnce(float x,float y, float z,bool isRad = false);

		void RotateOnce(glm::vec3 direction, glm::vec3 axis);
	};

}