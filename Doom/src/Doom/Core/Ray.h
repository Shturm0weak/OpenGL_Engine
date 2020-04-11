#pragma once

namespace Doom {

	struct Hit {
		glm::vec2 point;
		Collision* Object;
		
	};
		class DOOM_API Ray {
		private:
			float maxLength = 100;
		public:
			Ray(glm::vec2 start, glm::vec2 direction, float length);
			glm::vec2 direction;
			glm::vec2 start;
			glm::vec2 end = glm::vec2(0.f);
			bool Raycast(Hit& hit,float length);

			void SetStart(glm::vec2 start);
			void SetDirection(glm::vec2 end);
	};

}