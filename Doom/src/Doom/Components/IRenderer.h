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

		glm::vec4 m_Color = COLORS::White;
		Shader* m_Shader = nullptr;
		bool m_Emissive = false;
		RenderType m_RenderType = RenderType::TYPE_2D;

		Irenderer() {}
		virtual ~Irenderer() {}

		static Component* Create() { return new Irenderer(); }

		virtual void Render() {}
	};

}