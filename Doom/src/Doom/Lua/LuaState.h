#pragma once

namespace Doom {

	class DOOM_API LuaState {
	public:

		lua_State* m_L = nullptr;
		std::string m_FilePath;
	
		//Check Lua
		bool CL(int error); 
		static bool CL(LuaState* l, int error);
	
		//Transform
		static void RegisterTransformFunctions(LuaState* l);
		static int LuaGetTransformProperty(lua_State* l);
		static int LuaGetPosition(lua_State* l);
		static int LuaTranslate(lua_State* l);
		static int LuaGetRotation(lua_State* l);
		static int LuaRotate(lua_State* l);
		static int LuaGetScale(lua_State* l);
		static int LuaScale(lua_State* l);

		//GameObject
		static void RegisterGameObjectsFunctions(LuaState* l);
		static int LuaGetGameObjectProperty(lua_State* l);
		static int LuaGetThis(lua_State* l);
		static int LuaCreate(lua_State* l);
		static int LuaDestroyGameObject(lua_State* l);
		static int LuaGetTransform(lua_State* l);
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
	};

}