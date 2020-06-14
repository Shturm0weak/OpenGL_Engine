#pragma once

#include "Core/Core.h"
#include <iostream>
#include "glm/glm.hpp"
#include "Core/Window.h"

namespace Doom {

	class DOOM_API Application {
	public:
		int width = 0;
		int height = 0;
		bool Vsync = false;
		std::string name;

		Application(std::string name = "SandBox",int width = 800,int height = 600,bool Vsync = false) : name(name),width(width),height(height),Vsync(Vsync){}
		virtual ~Application() {}

		virtual void OnStart(){}
		virtual void OnUpdate(){}
		virtual void OnClose(){}
		virtual void OnGuiRender() {}
	};

}