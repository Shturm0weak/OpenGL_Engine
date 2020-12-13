#pragma once
#include "../pch.h"
#include "../Enums/ColoredOutput.h"
#include <ImGui/imgui.h>
#include <ImGui/examples/imgui_impl_glfw.h>
#include <ImGui/examples/imgui_impl_opengl3.h>
#include "../Objects/Camera.h"
#include "../Application.h"

namespace Doom {

	class DOOM_API Window {
	private:

		static GLFWwindow* s_Window;
		static Camera* s_Camera;
		static float s_ScrollYOffset;
		static bool s_CursorStateChanged; //reset every frame

		Window() {}

	public:

		static ImGuiContext* s_ImGuiContext;
		static ImGuiIO* s_ImGuiIO;

		static void ClampCursorPos();
		static void SetCursorPos(glm::dvec2 pos);
		static int* GetSize();
		static int GetCursorMode();
		static void HideCursor();
		static void ShowCursor();
		static int Init(const char* Label, float width, float height, bool vsync);
		static void SetCurrentContext(GLFWwindow* context);
		static glm::dvec2 GlfwGetMousePos();
		static float GetFPS();
		static void DisableCursor();
		inline static void Exit() { glfwSetWindowShouldClose(Window::GetWindow(), GLFW_TRUE); }
		inline static Camera& GetCamera() { return *s_Camera; }
		inline static float& GetScrollYOffset() { return s_ScrollYOffset; }
		inline static bool GetCursorStateChanged() { return s_CursorStateChanged; }
		inline static GLFWwindow* GetWindow() { return s_Window; }

		friend class Doom::EntryPoint;
	};

}