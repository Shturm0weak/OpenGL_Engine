#pragma once

#include "../Core/Core.h"
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API DirectionalLight : public Component {
	public:
		static std::vector<DirectionalLight*> dirLights;

		vec3 dir;

		glm::vec3 color = { 1.0f,1.0f,1.0f };

		DirectionalLight();
		~DirectionalLight();
	};

}