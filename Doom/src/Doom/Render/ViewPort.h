#pragma once

namespace Doom {

	class DOOM_API ViewPort {
	private:

		glm::vec2 size = glm::vec2(0, 0);
		glm::dvec2 staticMousePos = glm::vec2(0, 0);
		glm::dvec2 mousePosW = glm::vec2(0, 0);
		glm::dvec2 mousePosS = glm::vec2(0, 0);
		glm::vec2 viewportPos = glm::vec2(0, 0);
		glm::dvec2 cursorPos = glm::dvec2(0, 0);

	public:

		bool viewportResized = false;
		bool toolOpen = true;
		bool IsHovered = false;
		bool IsActive = false;

		static ViewPort* ViewPort::GetInstance();

		inline glm::vec2 GetSize() { return size; }
		inline void SetSize(float x, float y) { size.x = x; size.y = y; }
		inline void SetSize(glm::vec2 _size) { size = _size; }

		inline glm::vec2 GetViewPortPos() { return viewportPos; }
		inline void SetViewPortPos(float x, float y) { viewportPos.x = x; viewportPos.y = y; }
		inline void SetViewPortPos(glm::vec2 _viewportPos) { viewportPos = _viewportPos; }

		void Update();

	private:

		void GetMousePositionToWorldSpaceImpl();
		void GetStaticMousePositionImpl();
		void GetMousePositionToScreenSpaceImpl();

	public:

		inline glm::dvec2 GetMousePositionToScreenSpace() {return mousePosS;}
		inline glm::dvec2 GetStaticMousePosition() { return staticMousePos; }
		inline glm::dvec2 GetMousePositionToWorldSpace() { return mousePosW; }

		glm::dvec2 GetFromWorldToScreenSpaceImpl(glm::vec2 pos);
		glm::dvec2 GetFromWorldToScreenSpace(float x, float y);
	};

}