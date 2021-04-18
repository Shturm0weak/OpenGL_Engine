#pragma once

namespace Doom {

	class DOOM_API ViewPort {
	private:

		glm::dvec2 m_PrevMousePos = glm::vec2(0, 0);
		glm::dvec2 m_MouseDragDelta = glm::vec2(0,0);
		glm::dvec2 m_CursorPos = glm::dvec2(0, 0);
		glm::dvec2 m_StaticMousePos = glm::vec2(0, 0);
		glm::dvec2 m_MousePosW = glm::vec2(0, 0);
		glm::dvec2 m_MousePosS = glm::vec2(0, 0);
		glm::vec2 m_ViewportPos = glm::vec2(0, 0);
		glm::vec2 m_Size = glm::vec2(0, 0);

		int m_GizmoOperation = -1;

		void GetMousePositionToWorldSpaceImpl();
		void GetStaticMousePositionImpl();
		void GetMousePositionToScreenSpaceImpl();

	public:

		bool m_IsViewportResized = false;
		bool m_IsToolOpen = true;
		bool m_IsHovered = false;
		bool m_IsActive = false;

		static ViewPort& ViewPort::GetInstance();

		void RecalculateMouseCoords();
		void Update();
		glm::dvec2 GetFromWorldToScreenSpaceImpl(glm::vec2 pos);
		glm::dvec2 GetFromWorldToScreenSpace(float x, float y);
		glm::dvec2 GetMousePositionToScreenSpace() { return m_MousePosS; }
		glm::dvec2 GetStaticMousePosition() { return m_StaticMousePos; }
		glm::dvec2 GetMousePositionToWorldSpace() { return m_MousePosW; }
		glm::dvec2 GetMouseDragDelta() { return m_MouseDragDelta; }
		glm::vec2 GetViewPortPos() const { return m_ViewportPos; }
		glm::vec2 GetSize() const { return m_Size; }
		void SetSize(float x, float y) { m_Size.x = x; m_Size.y = y; }
		void SetSize(glm::vec2 size) { m_Size = size; }
		void SetViewPortPos(float x, float y) { m_ViewportPos.x = x; m_ViewportPos.y = y; }
		void SetViewPortPos(glm::vec2 viewportPos) { m_ViewportPos = viewportPos; }
		float GetAspectRatio() {return m_Size.x / m_Size.y;}

		friend class Doom::Editor;
		friend class Doom::Window;
	};

}