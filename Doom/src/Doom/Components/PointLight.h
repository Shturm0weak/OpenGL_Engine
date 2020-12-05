#pragma once

#include "../Core/Core.h"
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API PointLight : public Component {
	public:
		static std::vector<PointLight*> s_PointLights;
		
		glm::vec3 color = { 1.0f,1.0f,1.0f };

		float m_Constant = 1.0f;
		float m_Linear = 0.09f;
		float m_Quadratic = 0.032f;

		PointLight();
		~PointLight();
	};

}