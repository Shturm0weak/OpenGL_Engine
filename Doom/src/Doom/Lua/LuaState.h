#pragma once

namespace Doom {

	class DOOM_API LuaState {
	public:

		lua_State* m_L = nullptr;
		std::string m_FilePath;
	
		//Check Lua
		bool CL(int error); 
		static bool CL(LuaState* l, int error);
	
		static int LuaGetPosition(lua_State* l);
		static int LuaTranslate(lua_State* l);

		static int LuaGetScale(lua_State* l);
		static int LuaScale(lua_State* l);
	public:

		GameObject* m_Go = nullptr;
		static std::vector<LuaState*> luaStates;

		static const char* GetLuaGlobalName(GameObject* go);
		static LuaState* GetLuaOwner(lua_State* l);
		static void RegisterFunction(LuaState* l);
		void OnUpdate(float dt);
		void OnStart();

		LuaState(const char* filePath);
		~LuaState();
	};

}