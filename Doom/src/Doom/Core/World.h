#pragma once

namespace Doom {

	class DOOM_API World {
	public:
		static std::vector <GameObject*> objects;

		static void ProccessLuaStates();
		static void StartLuaStates();
		static void DeleteObject(int id);
		static void DeleteAll();
		static GameObject* SelectObject();
		static void ShutDown();
		static void PopBack();
		static void SelectObject3D();
		static unsigned int GetAmountOfObjects();
		static int obj_id;
		static int col_id;
		static GameObject* CreateGameObject();
		static bool ObjectCollided(std::vector<glm::vec2>& p, int i);

		friend class RectangleCollider2D;
	};
}