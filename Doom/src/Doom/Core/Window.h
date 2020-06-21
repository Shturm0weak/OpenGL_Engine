#pragma once
#include "../pch.h"
#include "ColoredOutput.h"
#include <ImGui/imgui.h>
#include <ImGui/examples/imgui_impl_glfw.h>
#include <ImGui/examples/imgui_impl_opengl3.h>
#include "../Render/OrthographicCamera.h"

namespace Doom {

	class DOOM_API Window {
	public:
		struct Position {
			float x = 0.f;
			float y = 0.f;

			Position(float _x, float _y) {
				x = _x;
				y = _y;
			}
			Position() {
				x = 0;
				y = 0;
			}

		};
	private:
		static GLFWwindow* m_window;
		static Camera m_camera;

		Window() {}

	public:
		static ImGuiContext* imGuiContext;
		static ImGuiIO* io;
		static Camera& GetCamera() {
			return m_camera;
		}

		static int* GetProps() {
			static int props[2];
			return props;
		}

		static void Exit() {
			glfwSetWindowShouldClose(Window::GetWindow(), GLFW_TRUE);
		}

		static int Init(const char* Label,float width, float height,bool vsync) {
			if (!glfwInit())
				return -1;
			GetProps()[0] = width;
			GetProps()[1] = height;

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
				int* props = Window::GetProps();
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

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glEnable(GL_CULL_FACE);
			return 0;
		}
		inline static GLFWwindow* GetWindow() { return m_window; }
		static Position GetMousePositionToWorldSpace() {
			static Position mousePos;
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			mousePos.x = (ImGui::GetMousePosOnOpeningCurrentPopup().x / GetProps()[0] - 0.5) * 32. * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().x;
			mousePos.y = (-(ImGui::GetMousePosOnOpeningCurrentPopup().y / GetProps()[1]) + 0.5) * 18. * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().y;
			return mousePos;
		}
		static Position GetMousePositionToScreenSpace() {
			static Position mousePos;
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			mousePos.x = (ImGui::GetMousePosOnOpeningCurrentPopup().x   / GetProps()[0]  - 0.5)  * 32.;
			mousePos.y = (-(ImGui::GetMousePosOnOpeningCurrentPopup().y / GetProps()[1]) + 0.5) * 18.;
			return mousePos;
		}

		static float GetFPS() {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			return ImGui::GetIO().Framerate;
		}
	};

}