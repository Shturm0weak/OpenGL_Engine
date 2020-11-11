#pragma once

#include "../Core/Core.h"
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API DirectionalLight : public Component {
	private:
		glm::mat4 lightSpaceMatrix;
	public:
		static std::vector<DirectionalLight*> dirLights;

		vec3 dir;
		float intensity = 1.0;
		glm::vec3 color = { 1.0f,1.0f,1.0f };

		glm::mat4 lightProjection;
	
		static glm::mat4 GetLightSpaceMatrix();
		void UpdateLightMatrix();

		DirectionalLight();
		~DirectionalLight();
	};

}