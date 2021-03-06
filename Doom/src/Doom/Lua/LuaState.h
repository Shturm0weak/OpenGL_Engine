#pragma once

namespace Doom {

	class DOOM_API LuaState {
	public:

		lua_State* m_L = nullptr;
		std::string m_FilePath;
	
		//Check Lua
		bool CL(int error); 
		static bool CL(LuaState* l, int error);
	
		//Color
		static int LuaGetColor(lua_State* l);
		static int LuaSetColor(lua_State* l);

		//Transform
		static void RegisterTransformFunctions(LuaState* l);
		static int LuaGetTransformProperty(lua_State* l);
		static int LuaGetTransformPosition(lua_State* l);
		static int LuaTransformTranslate(lua_State* l);
		static int LuaGetTransformRotation(lua_State* l);
		static int LuaTransformRotate(lua_State* l);
		static int LuaGetTransformScale(lua_State* l);
		static int LuaTransformScale(lua_State* l);

		//Raycast
		static void RegisterRaycastFunctions(LuaState* l);

		//Input
		static void RegisterInputFunctions(LuaState* l);
		static int LuaIsMousePressed(lua_State* l);
		static int LuaIsMouseDown(lua_State* l);
		static int LuaIsMouseReleased(lua_State* l);
		static int LuaIsKeyDown(lua_State* l);
		static int LuaIsKeyPressed(lua_State* l);

		//Camera
		static void RegisterCameraFunctions(LuaState* l);
		static int LuaGetCameraProperty(lua_State* l);
		static int LuaGetCameraPosition(lua_State* l);
		static int LuaSetCameraPosition(lua_State* l);
		static int LuaGetCameraRotation(lua_State* l);
		static int LuaSetCameraRotation(lua_State* l);
		static int LuaGetCameraForwardV(lua_State* l);
		static int LuaGetCameraMouseDirV(lua_State* l);

		//GameObject
		static void RegisterGameObjectsFunctions(LuaState* l);
		static int LuaGameObjectDestructor(lua_State* l);
		static int LuaGetGameObjectProperty(lua_State* l);
		static int LuaFindGameObjectById(lua_State* l);
		static int LuaGetComponent(lua_State* l);
		static int LuaGetThis(lua_State* l);
		static int LuaCreate(lua_State* l);
		static int LuaDestroyGameObject(lua_State* l);
		//static int LuaGetTransform(lua_State* l);
	public:

		GameObject* m_Go = nullptr;
		static std::vector<LuaState*> s_LuaStates;

		static const char* GetLuaGlobalName(GameObject* go);
		static LuaState* GetLuaOwner(lua_State* l);
		static void RegisterFunction(LuaState* l);
		void OnUpdate(float dt);
		void OnStart();

		LuaState(const char* filePath);
		~LuaState();

		friend class Doom::Editor;
	};

}