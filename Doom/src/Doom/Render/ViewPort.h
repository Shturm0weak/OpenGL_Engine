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

		static ViewPort* ViewPort::Instance() {
			static ViewPort instance;
			return &instance;
		}

		inline glm::vec2 GetSize() { return size; }
		inline void SetSize(float x, float y) { size.x = x; size.y = y; }
		inline void SetSize(glm::vec2 _size) { size = _size; }

		inline glm::vec2 GetViewPortPos() { return viewportPos; }
		inline void SetViewPortPos(float x, float y) { viewportPos.x = x; viewportPos.y = y; }
		inline void SetViewPortPos(glm::vec2 _viewportPos) { viewportPos = _viewportPos; }

		glm::dvec2 GetMousePositionToWorldSpace() {
			glfwGetCursorPos(Window::GetWindow(), &cursorPos.x, &cursorPos.y);
			mousePos.x = ((((cursorPos.x - viewportPos.x) / (Window::GetProps()[0] - 1))) - 0.5) * 32 * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().x;
			mousePos.y = (((-((cursorPos.y + (Window::GetProps()[1] - size.y)) / Window::GetProps()[1])) + 0.5)) * 18 * Window::GetCamera().GetZoomLevel() + Window::GetCamera().GetPosition().y;
			return mousePos;
		};
		glm::dvec2 GetMousePositionToScreenSpace() {

			glfwGetCursorPos(Window::GetWindow(), &cursorPos.x, &cursorPos.y);
			mousePos.x = ((((cursorPos.x - viewportPos.x) / (Window::GetProps()[0] - 1))) - 0.5 ) * 32;
			mousePos.y = (((-((cursorPos.y + (Window::GetProps()[1] - size.y)) / Window::GetProps()[1])) + 0.5)) * 18;
			return mousePos;
		};
	};

}