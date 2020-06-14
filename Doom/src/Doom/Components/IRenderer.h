#pragma once
#include "../Core/Core.h"
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API Irenderer : public Component{
	public:
		Irenderer() {}
		virtual ~Irenderer() {}

		virtual void Render() {}
	};

}