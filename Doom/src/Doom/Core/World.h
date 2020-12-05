#pragma once

namespace Doom {

	class DOOM_API World {
	public:

		static std::vector <GameObject*> s_GameObjects;

		static int s_ObjId;
		static int s_ColId;

		static void ProccessLuaStates();
		static void StartLuaStates();
		static void DeleteObject(int id);
		static void DeleteAll();
		static void ShutDown();
		static void PopBack();
		static void SelectObject3D();
		static unsigned int GetAmountOfObjects();
		static bool ObjectCollided(std::vector<glm::vec2>& p, int i);
		static GameObject* SelectObject();
		static GameObject* CreateGameObject();

		friend class RectangleCollider2D;
	};
}