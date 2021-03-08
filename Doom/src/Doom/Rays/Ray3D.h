#pragma once

#include "../Core/Core.h"
namespace Doom {

	class DOOM_API Ray3D {
	public:

		struct Hit {
			glm::dvec3 m_Point;
			CubeCollider3D* m_Object = nullptr;
			double m_Distance = 0;
		};

		static std::vector<std::string> m_IgnoreMask;

		static std::map<double, CubeCollider3D*> RayCast(glm::dvec3 start, glm::dvec3 dir, Hit* hit, double length, bool AABB = true, std::vector<std::string> ignoreMask = Ray3D::m_IgnoreMask);
		static bool IntersectTriangle(glm::dvec3 start, glm::dvec3 dir, Hit* hit, double length, glm::dvec3& a, glm::dvec3& b, glm::dvec3& c, glm::dvec3& planeNorm);
		static void Normilize(glm::dvec3& vector);
		static bool IntersectBoxAABB(glm::dvec3 start, glm::dvec3 dir, Hit * hit, double length, CubeCollider3D* c);
		static bool IntersectBoxOBB(glm::dvec3 start, glm::dvec3 dir, Hit * hit, double length, CubeCollider3D* c);
	};

}