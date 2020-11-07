#pragma once
#include "Core.h"
namespace Doom {

	class DOOM_API Ray3D {
	public:

		struct Hit {
			glm::vec3 point;
			CubeCollider3D* Object = nullptr;
			float distance = 0;
		};

		static void sortMap(std::map<float, CubeCollider3D*>& M);
		static std::map<float, CubeCollider3D*> RayCast(glm::vec3 start, glm::vec3 dir, Hit* hit, float length, bool AABB = true);
		static bool IntersectTriangle(glm::vec3 start, glm::vec3 dir, Hit* hit, float length, glm::vec3& a, glm::vec3& b, glm::vec3& c, glm::vec3& planeNorm);
		static void Normilize(glm::vec3& vector);
		static bool IntersectBoxAABB(glm::vec3 start, glm::vec3 dir, Hit * hit, float length, CubeCollider3D* c);
		static bool IntersectBoxOBB(glm::vec3 start, glm::vec3 dir, Hit * hit, float length, CubeCollider3D* c);
	};

}