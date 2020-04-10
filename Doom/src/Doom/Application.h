#pragma once

#include "Core.h"
#include <iostream>
#include "glm/glm.hpp"
#include "Core/Window.h"

namespace Doom {

	class DOOM_API Application {
	public:
		Application(){}
		virtual ~Application() {}

		virtual void OnStart(){}
		virtual void OnUpdate(){}
		virtual void OnClose(){}
	};

}