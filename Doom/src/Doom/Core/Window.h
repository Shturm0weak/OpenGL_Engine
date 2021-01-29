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

		GLFWwindow* s_Window = nullptr;
		Camera* s_Camera = new Camera();
		float s_ScrollYOffset = 0;
		bool s_CursorStateChanged = false; //reset every frame

		Window() {}
		Window(const Window&) = delete;
		Window& operator=(const Window&) { return *this; }

	public:

		ImGuiContext* s_ImGuiContext = nullptr;
		ImGuiIO* s_ImGuiIO = nullptr;

		static Window& GetInstance();
		void ClampCursorPos();
		void SetCursorPos(glm::dvec2 pos);
		int* GetSize();
		int GetCursorMode();
		void HideCursor();
		void ShowCursor();
		int Init(const char* Label, float width, float height, bool vsync);
		void SetCurrentContext(GLFWwindow* context);
		glm::dvec2 GlfwGetMousePos();
		float GetFPS();
		void DisableCursor();
		void Exit() { glfwSetWindowShouldClose(s_Window, GLFW_TRUE); }
		Camera& GetCamera() { return *s_Camera; }
		float& GetScrollYOffset() { return s_ScrollYOffset; }
		bool GetCursorStateChanged() { return s_CursorStateChanged; }
		GLFWwindow* GetWindow() { return s_Window; }

		friend class Doom::EntryPoint;
	};

}