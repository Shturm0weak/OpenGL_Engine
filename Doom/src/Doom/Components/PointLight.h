#pragma once

#include "../Core/Core.h"
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API PointLight : public Component {
	private:

		void Copy(const PointLight& rhs);
	public:

		static std::vector<PointLight*> s_PointLights;
		
		glm::vec3 m_Color = { 1.0f,1.0f,1.0f };

		float m_Constant = 1.0f;
		float m_Linear = 0.09f;
		float m_Quadratic = 0.032f;

		void operator=(const PointLight& rhs);

		static Component* Create();

		PointLight(const PointLight& rhs);
		PointLight();
		~PointLight();
	};

}