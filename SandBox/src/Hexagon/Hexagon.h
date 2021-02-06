#pragma once

#include "Application.h"

namespace Doom {

	class Hexagon : public Application {
	private:
		float width = 16;
		float height = 16;
	public:

		Hexagon(std::string name = "Hexagon", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_3D, width, height, Vsync) {}

		void PerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput);

		glm::vec4 Colors(float value);

		void OnStart();
		void OnUpdate();
		void OnClose();
		void OnGuiRender();
	};

}