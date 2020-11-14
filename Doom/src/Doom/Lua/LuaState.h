#pragma once

namespace Doom {

	class DOOM_API LuaState {
	public:
		lua_State* l = nullptr;
		std::string filePath;
	
		//Check Lua
		bool CL(int error); 
		static bool CL(LuaState* l, int error);
	
		static int LuaGetPosition(lua_State* l);
		static int LuaTranslate(lua_State* l);

		static int LuaGetScale(lua_State* l);
		static int LuaScale(lua_State* l);

	public:
		static void RegisterFunction(LuaState* l);
		GameObject* go = nullptr;
		static std::vector<LuaState*> luaStates;

		void OnUpdate(float dt);
		void OnStart();
		LuaState(const char* filePath);
		~LuaState();
		
		static const char* GetLuaGlobalName(GameObject* go);
		static LuaState* GetLuaOwner(lua_State* l);
	};

}