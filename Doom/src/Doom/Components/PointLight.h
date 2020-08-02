#pragma once

#include "../Core/Core.h"
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API PointLight : public Component {
	public:
		static std::vector<PointLight*> pLights;
		
		float constant = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;

		glm::vec3 color = {1.0f,1.0f,1.0f};

		PointLight();
		~PointLight();
	};

}