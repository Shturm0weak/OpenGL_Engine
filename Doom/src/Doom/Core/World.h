#pragma once

namespace Doom {

	class DOOM_API World {
	private:

		World& operator=(const World& rhs) { return *this; }
		World(const World&) = delete;
		World() {}
	public:

		std::vector <GameObject*> s_GameObjects;

		int s_ObjId = 0;
		int s_ColId = 0;
		std::mutex m_Mtx;

		static World& GetInstance();
		void UpdateLuaStates();
		void StartLuaStates();
		void DeleteObject(int id);
		void DeleteAll();
		void ShutDown();
		void PopBack();
		GameObject* SelectObject3D();
		unsigned int GetAmountOfObjects();
		bool ObjectCollided(std::vector<glm::vec2>& p, int i);
		GameObject* SelectObject();

		friend class RectangleCollider2D;
		friend class Instancing;
	};
}