#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "../ECS/Component.h"


namespace Doom {

	class Irenderer;
	class RectangleCollider2D;

	class DOOM_API Transform : public Component {
	private:

		Irenderer* sr = nullptr;
		RectangleCollider2D* col = nullptr;

		void RealVertexPositions();

		glm::vec3 m_Rotation = glm::vec3(0.0);
		glm::vec3 m_PrevPosition;

		glm::mat4 m_ScaleMat4 = glm::mat4(1.f);
		glm::mat4 m_PosMat4   = glm::mat4(1.f);
		glm::mat4 m_ViewMat4  = glm::mat4(1.f);

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

	public:

		Transform();
		~Transform() {}

		glm::vec3 GetRotation();
		glm::vec3 GetPosition();
		glm::vec3 GetScale();

		void Move(glm::vec3 dir);
		void Rotate(glm::vec3 angles);
		void Scale(glm::vec3 scale);
		void Translate(glm::vec3 pos);
		void RotateOnce(glm::vec3 angles, bool isRad = false);

		void Move(float x = 0, float y = 0, float z = 0);
		void Rotate(float x, float y, float z );
		void Scale(float x, float y,float z = 0);
		void Translate(float x = 0, float y = 0,float z = 0);
		void RotateOnce(float x,float y, float z,bool isRad = false);

		void RotateOnce(glm::vec3 direction, glm::vec3 axis);
	};

}
#endif