#pragma once
#include "../pch.h"
#include "ColoredOutput.h"
#include <ImGui/imgui.h>
#include <ImGui/examples/imgui_impl_glfw.h>
#include <ImGui/examples/imgui_impl_opengl3.h>
#include "../Render/Camera.h"
#include "../Application.h"

namespace Doom {

	class DOOM_API Window {
	private:
		static GLFWwindow* m_window;
		static Camera* m_camera;

		Window() {}

	public:
		static ImGuiContext* imGuiContext;
		static ImGuiIO* io;
		static Camera& GetCamera() {
			return *m_camera;
		}

		static int* GetSize() {
			static int size[2];
			return size;
		}

		static void Exit() {
			glfwSetWindowShouldClose(Window::GetWindow(), GLFW_TRUE);
		}

		static int Init(const char* Label,float width, float height,bool vsync) {
			if (!glfwInit())
				return -1;
			GetSize()[0] = width;
			GetSize()[1] = height;

			m_window = glfwCreateWindow(width, height, Label, NULL, NULL);
			if (!GetWindow())
			{
				glfwTerminate();
				return -1;
			}

			glfwMakeContextCurrent(GetWindow());

			if (glewInit() != GLEW_OK) {
				std::cout << "error" << std::endl;
				return -1;
			}
			std::cout << BOLDGREEN << "Initialized window" << RESET << std::endl;

			glfwSwapInterval(vsync); // Enable vsync

			glfwSetWindowSizeCallback(Window::GetWindow(), [](GLFWwindow* window, int width, int height) {
				int* props = Window::GetSize();
				props[0] = width;
				props[1] = height;
				EventSystem::GetInstance()->SendEvent("OnWindowResize", nullptr, props);
			});
			imGuiContext = ImGui::CreateContext();
			io = &ImGui::GetIO();
			(void)io;
			ImGui_ImplGlfw_InitForOpenGL(Window::GetWindow(), true);
			ImGui_ImplOpenGL3_Init("#version 330");

			ImGui::StyleColorsClassic();
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			return 0;
		}

		inline static GLFWwindow* GetWindow() { return m_window; }
	
		static float GetFPS() {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			return ImGui::GetIO().Framerate;
		}
	};

}