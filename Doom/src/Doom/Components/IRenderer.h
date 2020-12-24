#pragma once
#include "../Core/Core.h"
#include "../ECS/Component.h"
#include "../Render/TextureAtlas.h"
#include "../Enums/ColorsRGBA.h"

namespace Doom {

	enum DOOM_API RenderType {
		TYPE_2D,
		TYPE_3D,
	};

	class DOOM_API Irenderer : public Component{
	public:

		glm::mat4 m_MVP = glm::mat4(1.f);
		glm::vec4 m_Color = COLORS::White;
		TextureAtlas* m_TextureAtlas = nullptr;
		Shader* m_Shader = nullptr;
		RenderType m_RenderType = RenderType::TYPE_2D;

		Irenderer() {}
		virtual ~Irenderer() {}

		virtual void Render() {}

		void SetColor(glm::vec4 color) {
			this->m_Color = color;
		}

		float* GetColor() const {
			float* color = new float[4];
			for (unsigned int i = 0; i < 4; i++)
			{
				color[i] = this->m_Color[i];
			}
			return color;
		}

		virtual void Update(glm::vec3 pos) {}
	};

}