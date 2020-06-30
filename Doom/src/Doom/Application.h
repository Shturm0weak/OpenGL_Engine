#pragma once

#include "Core/Core.h"
#include <iostream>
#include "glm/glm.hpp"
#include "Core/Window.h"

namespace Doom {

	enum DOOM_API ApplicationType {
		TYPE_2D = 0,
		TYPE_3D = 1,
	};

	class DOOM_API Application {
	public:
		int width = 0;
		int height = 0;
		bool Vsync = false;
		std::string name;
		ApplicationType type = TYPE_2D;

		Application(std::string name = "SandBox", ApplicationType type = TYPE_2D, int width = 800, int height = 600, bool Vsync = false) 
			: name(name), type(type), width(width), height(height), Vsync(Vsync) {}
		void Init();
		virtual ~Application() {}

		virtual void OnStart(){}
		virtual void OnUpdate(){}
		virtual void OnClose(){}
		virtual void OnGuiRender() {}
	};

}