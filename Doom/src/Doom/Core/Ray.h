#pragma once

namespace Doom {

	struct Hit {
		glm::vec2 point;
		Collision* Object;

	};
	class DOOM_API Ray {
	public:
		std::vector<std::string> ignoreMask;
		bool Doom::Ray::RayCastObjects(Hit & hit, glm::vec2 start, glm::vec2 direction);
		Ray(glm::vec2 start, glm::vec2 direction, float length);
		float length;
		glm::vec2 direction;
		glm::vec2 start;
		glm::vec2 end = glm::vec2(0.f);
		bool Raycast(Hit& hit, float length, glm::vec2 start, glm::vec2 direction, std::vector<std::string> ignoreMask);

		void SetStart(glm::vec2 start);
		void SetDirection(glm::vec2 end);
		glm::vec2* LineIntersect(glm::vec2 corner, glm::vec2 nextCorner, glm::vec2 from, glm::vec2 to);
		float Distance(glm::vec2 start, glm::vec2 end);
		static void Normilize(glm::vec2& vector);

		void Rotate(float angle);
	};

}