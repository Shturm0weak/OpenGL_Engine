#pragma once

#include "Core/Core.h"
#include <iostream>
#include "glm/glm.hpp"
#include "Core/Window.h"

namespace Doom {

	class DOOM_API Application {
	public:

		std::string m_Name;
		int m_Width = 0;
		int m_Height = 0;
		bool m_Vsync = false;

		Application(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false)
			: m_Name(name), m_Width(width), m_Height(height), m_Vsync(Vsync) {}
		
		virtual ~Application() {}

		void Init() const;

		virtual void OnStart(){}
		virtual void OnUpdate(){}
		virtual void OnClose(){}
		virtual void OnGuiRender() {}
	};

}