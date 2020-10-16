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

		static void RayCast(glm::vec3 start, glm::vec3 dir,Hit* hit,float length);
	};

}