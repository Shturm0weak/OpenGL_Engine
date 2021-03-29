#pragma once

#include "../Core/Core.h"
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API SpotLight : public Component {
	private:

		void Copy(const SpotLight& rhs);
	public:

		static std::vector<SpotLight*> s_SpotLights;

		glm::vec3 m_Color = { 1.0f,1.0f,1.0f };
		
		float m_InnerCutOff = glm::cos(glm::radians(12.5f));
		float m_OuterCutOff = glm::cos(glm::radians(17.5f));
		float m_Constant = 1.0f;
		float m_Linear = 0.09f;
		float m_Quadratic = 0.032f;

		void operator=(const SpotLight& rhs);

		static Component* Create();

		SpotLight(const SpotLight& rhs);
		SpotLight();
		~SpotLight();
	};

}