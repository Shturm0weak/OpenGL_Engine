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

		Window() {}

	public:

		static ImGuiContext* s_ImGuiContext;
		static ImGuiIO* s_ImGuiIO;

		static Camera& GetCamera() {
			return *s_Camera;
		}

		static int* GetSize() {
			static int size[2];
			return size;
		}

		static void Exit() {
			glfwSetWindowShouldClose(Window::GetWindow(), GLFW_TRUE);
		}

		static float& GetScrollYOffset() { return s_ScrollYOffset; }

		static int Init(const char* Label,float width, float height,bool vsync) {
			if (!glfwInit())
				return -1;
			GetSize()[0] = width;
			GetSize()[1] = height;

			s_Window = glfwCreateWindow(width, height, Label, NULL, NULL);
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

			glfwSetScrollCallback(Window::GetWindow(), [](GLFWwindow* window, double xoffset, double yoffset) {
				Window::GetScrollYOffset() = yoffset;
			});

			glfwSetWindowSizeCallback(Window::GetWindow(), [](GLFWwindow* window, int width, int height) {
				int* props = Window::GetSize();
				props[0] = width;
				props[1] = height;
				EventSystem::GetInstance()->SendEvent(EventType::ONWINDOWRESIZE, nullptr, props);
			});
			s_ImGuiContext = ImGui::CreateContext();
			s_ImGuiIO = &ImGui::GetIO();
			(void)s_ImGuiIO;
			ImGui_ImplGlfw_InitForOpenGL(Window::GetWindow(), true);
			ImGui_ImplOpenGL3_Init("#version 330");

			ImGui::StyleColorsClassic();
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			return 0;
		}

		inline static GLFWwindow* GetWindow() { return s_Window; }
	
		static float GetFPS() {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			return ImGui::GetIO().Framerate;
		}
	};

}