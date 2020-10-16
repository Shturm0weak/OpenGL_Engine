#pragma once
#include "Render/Renderer.h"
#include "Render/Batch.h"
#include "Render/Gui.h"
#include <ImGui/imgui.h>
#include <ImGui/examples/imgui_impl_glfw.h>
#include <ImGui/examples/imgui_impl_opengl3.h>
#include "Render/ViewPort.h"
#include "Application.h"

namespace Doom {

	class DOOM_API EntryPoint {
	public:
		int size[2];
		Doom::Application* app = nullptr;
		EntryPoint(Doom::Application* app);
		~EntryPoint();
		void Run();
	};

}