#pragma once
#include "../pch.h"
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
		static OrthographicCamera m_camera;

		Window() {}

	public:

		static OrthographicCamera& GetCamera() {
			return m_camera;
		}

		static int* GetProps() {
			static int props[2];
			return props;
		}

		static int Init(int width, int height) {
			if (!glfwInit())
				return -1;
			GetProps()[0] = width;
			GetProps()[1] = height;

			m_window = glfwCreateWindow(width, height, "DoomEngine", NULL, NULL);
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
			std::cout << "Initialized window" << std::endl;

			glfwMakeContextCurrent(Window::GetWindow());
			glfwSwapInterval(0); // Enable vsync

			glfwSetWindowSizeCallback(Window::GetWindow(), [](GLFWwindow* window, int width, int height) {
				int* props = Window::GetProps();
				props[0] = width;
				props[1] = height;
				EventSystem::Instance()->SendEvent("OnWindowResize", nullptr, props);
			});

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			(void)io;
			ImGui_ImplGlfw_InitForOpenGL(Window::GetWindow(), true);
			ImGui_ImplOpenGL3_Init("#version 330");

			ImGui::StyleColorsDark();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glEnable(GL_DEPTH_TEST);
			// Фрагмент будет выводиться только в том, случае, если он находится ближе к камере, чем предыдущий
			//glDepthFunc(GL_LESS);

			return 0;
		}
		inline static GLFWwindow* GetWindow() { return m_window; }
		inline static Position GetMousePositionToWorldSpace() {
			static Position mousePos;
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			mousePos.x = (ImGui::GetMousePosOnOpeningCurrentPopup().x / GetProps()[0] - 0.5) * 32. * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().x;
			mousePos.y = (-(ImGui::GetMousePosOnOpeningCurrentPopup().y / GetProps()[1]) + 0.5) * 18. * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().y;
			return mousePos;
		}
		inline static Position GetMousePositionToScreenSpace() {
			static Position mousePos;
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			mousePos.x = (ImGui::GetMousePosOnOpeningCurrentPopup().x / GetProps()[0] - 0.5) * 32. * Window::GetCamera().GetZoomLevel();
			mousePos.y = (-(ImGui::GetMousePosOnOpeningCurrentPopup().y / GetProps()[1]) + 0.5) * 18. * Window::GetCamera().GetZoomLevel();
			return mousePos;
		}
	};

}