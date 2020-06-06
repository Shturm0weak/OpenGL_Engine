#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API Transform : public Component {
	private:

		Collision* col = nullptr;
		struct Position {
			float x = 0;
			float y = 0;
			float z = 0;
		};
		void init();
		void RealVertexPositions();
		friend class ComponentManager;
		float WorldVerPos[16];

	public:
		Transform();
		Position position;
		float angle = 0;
		void Move(float speedX = 0, float speedY = 0, float speedZ = 0);
		void Rotate(float angle, glm::vec3 axis);
		void Scale(float scaleX, float scaleY,float scaleZ = 0);
		void Translate(float x = 0, float y = 0,float z = 0);
		void RotateOnce(float angle, glm::vec3 axis);
		void RotateOnce(glm::vec3 direction, glm::vec3 axis);
	};

}
#endif