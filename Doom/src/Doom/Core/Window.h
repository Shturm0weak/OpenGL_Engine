#pragma once

#include <ImGui/imgui.h>
#include <ImGui/examples/imgui_impl_glfw.h>
#include <ImGui/examples/imgui_impl_opengl3.h>
#include "../Application.h"
#include "../Objects/Camera.h"

namespace Doom {

	class DOOM_API Window {
	private:

		GLFWwindow* s_Window = nullptr;
		Camera* s_Camera = new Camera();
		Application* s_Application = nullptr;
		float s_ScrollYOffset = 0;
		bool s_CursorStateChanged = false; //reset every frame

		Window() {}
		Window(const Window&) = delete;
		Window& operator=(const Window&) { return *this; }
		~Window();

	public:

		ImGuiContext* s_ImGuiContext = nullptr;
		ImGuiIO* s_ImGuiIO = nullptr;

		FrameBuffer* m_FrameBufferColor = nullptr;
		FrameBuffer* m_FrameBufferShadowMap = nullptr;
		FrameBuffer* m_FrameBufferBrightness = nullptr;
		std::vector<FrameBuffer*> m_FrameBufferBlur;

		static Window& GetInstance();
		void ClampCursorPos();
		void SetCursorPos(glm::dvec2 pos);
		int* GetSize();
		int GetCursorMode();
		void HideCursor();
		void ShowCursor();
		int Init(Application* app);
		void SetCurrentContext(GLFWwindow* context);
		glm::dvec2 GlfwGetMousePos();
		float GetFPS();
		void DisableCursor();
		void Exit() { glfwSetWindowShouldClose(s_Window, GLFW_TRUE); }
		float& GetScrollYOffset() { return s_ScrollYOffset; }
		bool GetCursorStateChanged() { return s_CursorStateChanged; }
		Application& GetApp() { return *s_Application; }
		Camera& GetCamera() { return *s_Camera; }
		GLFWwindow* GetWindow() { return s_Window; }

		friend class EntryPoint;
	};

}