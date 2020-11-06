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

		void init();
		void RealVertexPositions();

		float prevAngle;

		glm::vec3 prevPosition;

		glm::mat4 scale = glm::mat4(1.f);
		glm::mat4 pos = glm::mat4(1.f);
		glm::mat4 viewXscale = glm::mat4(1.f);
		glm::mat4 view = glm::mat4(1.f);

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

	public:
		Transform();
		~Transform() {}
		glm::vec3 rotation = glm::vec3(0.0);
		glm::vec3 position;
		float angleRad = 0;
		float angleDeg = 0;
		void Move(float speedX = 0, float speedY = 0, float speedZ = 0);
		void Rotate(float x, float y, float z );
		void Scale(float scaleX, float scaleY,float scaleZ = 0);
		void Translate(float x = 0, float y = 0,float z = 0);
		void RotateOnce(float x,float y, float z,bool isRad = false);
		void RotateOnce(glm::vec3 direction, glm::vec3 axis);
	};

}
#endif