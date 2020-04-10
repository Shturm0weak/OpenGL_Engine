#pragma once
#include "Application.h"
#include "Render/Renderer.h"
#include "Render/Batch.h"
#include "Render/Text.h"
#include <ImGui/imgui.h>
#include <ImGui/examples/imgui_impl_glfw.h>
#include <ImGui/examples/imgui_impl_opengl3.h>

namespace Doom {

	class DOOM_API EntryPoint {
	public:
		Doom::Application* app = nullptr;
		EntryPoint(Doom::Application* app);
		~EntryPoint();
		void Run();
	};

}