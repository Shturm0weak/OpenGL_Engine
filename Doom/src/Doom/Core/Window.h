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

		static GLFWwindow* m_Window;
		static Camera* m_Camera;
		static float m_ScrollYOffset;

		Window() {}

	public:

		static ImGuiContext* m_ImGuiContext;
		static ImGuiIO* m_ImGuiIO;

		static Camera& GetCamera() {
			return *m_Camera;
		}

		static int* GetSize() {
			static int size[2];
			return size;
		}

		static void Exit() {
			glfwSetWindowShouldClose(Window::GetWindow(), GLFW_TRUE);
		}

		static float& GetScrollYOffset() { return m_ScrollYOffset; }

		static int Init(const char* Label,float width, float height,bool vsync) {
			if (!glfwInit())
				return -1;
			GetSize()[0] = width;
			GetSize()[1] = height;

			m_Window = glfwCreateWindow(width, height, Label, NULL, NULL);
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
			m_ImGuiContext = ImGui::CreateContext();
			m_ImGuiIO = &ImGui::GetIO();
			(void)m_ImGuiIO;
			ImGui_ImplGlfw_InitForOpenGL(Window::GetWindow(), true);
			ImGui_ImplOpenGL3_Init("#version 330");

			ImGui::StyleColorsClassic();
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			return 0;
		}

		inline static GLFWwindow* GetWindow() { return m_Window; }
	
		static float GetFPS() {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			return ImGui::GetIO().Framerate;
		}
	};

}