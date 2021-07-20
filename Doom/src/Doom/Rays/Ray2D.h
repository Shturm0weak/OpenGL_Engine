#pragma once

namespace Doom {

	class DOOM_API Ray2D {
	public:

		struct Hit {
			glm::vec3 m_Point;
			RectangleCollider2D* m_Object;
		};

		std::vector<std::string> m_IgnoreMask;
		float m_Length;
		glm::vec2 m_Dir;
		glm::vec2 m_Start;
		glm::vec2 m_End = glm::vec2(0.f);

		bool Doom::Ray2D::RayCastObjects(Hit & hit, glm::vec2 start, glm::vec2 direction);
		bool Raycast(Hit& hit, float length, glm::vec2 start, glm::vec2 direction, std::vector<std::string> ignoreMask);
		void SetStart(glm::vec2 start);
		void SetDirection(glm::vec2 end);
		void Rotate(float angle);
		static glm::vec2* LineIntersect(glm::vec2 corner, glm::vec2 nextCorner, glm::vec2 from, glm::vec2 to);

		Ray2D(glm::vec2 start, glm::vec2 direction, float length);
	};

}