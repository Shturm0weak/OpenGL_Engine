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

		float WorldVerPos[16];
		float prevAngle;

		glm::vec3 prevPosition;

		friend class ComponentManager;
		friend class RectangleCollider2D;

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