#pragma once
#include "Application.h"

namespace Doom {

	class AimTrainer : public Application
	{
	private:

		void RayCast();
		void CameraMovement();

	public:

		AimTrainer(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_3D, width, height, Vsync) {}

		void OnStart();
		void OnUpdate();
		void OnClose();
		void OnGuiRender();
	};

}