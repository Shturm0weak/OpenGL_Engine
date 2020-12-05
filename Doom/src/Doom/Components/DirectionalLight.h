#pragma once

#include "../Core/Core.h"
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API DirectionalLight : public Component {
	private:

		glm::mat4 lightSpaceMatrix;
	public:

		static std::vector<DirectionalLight*> s_DirLights;

		glm::mat4 m_LightProjection;
		glm::vec3 m_Color = { 1.0f,1.0f,1.0f };
		vec3 m_Dir;
		float m_Intensity = 1.0;
	
		static glm::mat4 GetLightSpaceMatrix();
		void UpdateLightMatrix();

		DirectionalLight();
		~DirectionalLight();
	};

}