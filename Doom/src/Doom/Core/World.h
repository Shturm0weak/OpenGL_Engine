#pragma once

namespace Doom {

	class DOOM_API World {
	private:

		World& operator=(const World& rhs) { return *this; }
		World(const World&) = delete;
		World() {}
	public:

		std::vector <GameObject*> s_GameObjects;

		void* s_Application = nullptr;
		int s_ObjId = 0;
		int s_ColId = 0;

		static World& GetInstance();
		void ProccessLuaStates();
		void StartLuaStates();
		void DeleteObject(int id);
		void DeleteAll();
		void ShutDown();
		void PopBack();
		void SelectObject3D();
		unsigned int GetAmountOfObjects();
		bool ObjectCollided(std::vector<glm::vec2>& p, int i);
		GameObject* SelectObject();
		GameObject* CreateGameObject();

		friend class RectangleCollider2D;
	};
}