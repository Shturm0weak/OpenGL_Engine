#pragma once

namespace Doom {

	class DOOM_API ViewPort {
	private:

		glm::vec2 size = glm::vec2(0, 0);
		glm::dvec2 mousePos = glm::vec2(0, 0);
		glm::vec2 viewportPos = glm::vec2(0, 0);
		glm::dvec2 cursorPos = glm::dvec2(0, 0);

	public:

		bool viewportResized = false;
		bool toolOpen = true;
		bool IsHovered = false;
		bool IsActive = false;

		static ViewPort* ViewPort::GetInstance() {
			static ViewPort instance;
			return &instance;
		}

		inline glm::vec2 GetSize() { return size; }
		inline void SetSize(float x, float y) { size.x = x; size.y = y; }
		inline void SetSize(glm::vec2 _size) { size = _size; }

		inline glm::vec2 GetViewPortPos() { return viewportPos; }
		inline void SetViewPortPos(float x, float y) { viewportPos.x = x; viewportPos.y = y; }
		inline void SetViewPortPos(glm::vec2 _viewportPos) { viewportPos = _viewportPos; }

		void Update() {
			void* tex = reinterpret_cast<void*>(Window::GetCamera().frameBuffer->texture);

			ImGui::Begin("ViewPort", &ViewPort::GetInstance()->toolOpen, ImGuiWindowFlags_NoScrollbar);
			if (ImGui::IsWindowFocused())
				ViewPort::GetInstance()->IsActive = true;
			else
				ViewPort::GetInstance()->IsActive = false;

			if (ImGui::IsWindowHovered())
				ViewPort::GetInstance()->IsHovered = true;
			else
				ViewPort::GetInstance()->IsHovered = false;

			ViewPort::GetInstance()->SetViewPortPos(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
			if (ViewPort::GetInstance()->GetSize().x != ImGui::GetWindowSize().x || ViewPort::GetInstance()->GetSize().y != ImGui::GetWindowSize().y) {
				ViewPort::GetInstance()->SetSize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
				ViewPort::GetInstance()->viewportResized = true;
			}
			else
				ViewPort::GetInstance()->viewportResized = false;

			ImGui::GetWindowDrawList()->AddImage(tex, ImVec2(ViewPort::GetInstance()->GetViewPortPos().x,
				ViewPort::GetInstance()->GetViewPortPos().y), ImVec2(ViewPort::GetInstance()->GetViewPortPos().x + ViewPort::GetInstance()->GetSize().x, ViewPort::GetInstance()->GetViewPortPos().y + ViewPort::GetInstance()->GetSize().y), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
		}

		glm::dvec2 GetMousePositionToWorldSpace() {
			glfwGetCursorPos(Window::GetWindow(), &cursorPos.x, &cursorPos.y);
			mousePos.x = ((((cursorPos.x - viewportPos.x) / (Window::GetProps()[0] - 1))) - 0.5) * 32 * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().x;
			mousePos.y = (((-((cursorPos.y + (Window::GetProps()[1] - size.y)) / Window::GetProps()[1])) + 0.5)) * 18 * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().y;
			return mousePos;
		};

		glm::dvec2 GetStaticMousePosition() {

			glfwGetCursorPos(Window::GetWindow(), &cursorPos.x, &cursorPos.y);
			mousePos.x = ((((cursorPos.x - viewportPos.x) / (Window::GetProps()[0] - 1))) - 0.5 ) * 32;
			mousePos.y = (((-((cursorPos.y + (Window::GetProps()[1] - size.y)) / Window::GetProps()[1])) + 0.5)) * 18;
			return mousePos;
		};

		glm::dvec2 GetMousePositionToScreenSpace() {

			glfwGetCursorPos(Window::GetWindow(), &cursorPos.x, &cursorPos.y);
			mousePos.x = ((((cursorPos.x - viewportPos.x) / (Window::GetProps()[0] - 1))) - 0.5) * Window::GetProps()[0];
			mousePos.y = (((-((cursorPos.y + (Window::GetProps()[1] - size.y)) / Window::GetProps()[1])) + 0.5)) * Window::GetProps()[1];
			return mousePos;
		};

		glm::dvec2 GetFromWorldToScreenSpace(glm::vec2 pos) {
			return glm::dvec2((pos.x) / (16.* Window::GetCamera().GetZoomLevel()) * WIDTH, (pos.y) / (9. * Window::GetCamera().GetZoomLevel()) * HEIGHT);
		}

		glm::dvec2 GetFromWorldToScreenSpace(float x,float y) {
			return glm::dvec2((x) / (16.* Window::GetCamera().GetZoomLevel()) * WIDTH, (y) / (9. * Window::GetCamera().GetZoomLevel()) * HEIGHT);

		}
	};

}