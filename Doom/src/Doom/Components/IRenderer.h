#pragma once
#include "../Core/Core.h"
#include "../ECS/Component.h"
#include "../Render/TextureAtlas.h"

namespace Doom {

	class DOOM_API Irenderer : public Component{
	public:
		glm::mat4 scale = glm::mat4(1.f);
		glm::mat4 pos = glm::mat4(1.f);
		glm::mat4 viewXscale = glm::mat4(1.f);
		glm::mat4 MVP = glm::mat4(1.f);
		glm::mat4 view = glm::mat4(1.f);
		glm::vec4 color = COLORS::White;
		TextureAtlas* textureAtlas = nullptr;
		std::string renderType = "2D";

		Irenderer() {}
		virtual ~Irenderer() {}

		virtual void Render() {}

		void SetColor(glm::vec4 color) {
			this->color = color;
		}

		float* GetColor() const {
			float* color = new float[4];
			for (unsigned int i = 0; i < 4; i++)
			{
				color[i] = this->color[i];
			}
			return color;
		}

		Shader* shader = nullptr;

		virtual void Update(glm::vec3 pos) {}
	};

}