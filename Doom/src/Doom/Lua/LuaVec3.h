#pragma once

#include "lualib.h"

namespace Doom {
	
	class DOOM_API LuaVec3 {
	public:

		static void RegisterLuaVec3Functions(LuaState* l);
		static int Vec3(lua_State* l);
		static int Add(lua_State* l);
		static int Sub(lua_State* l);
		static int Mul(lua_State* l);
		static void PushVec3ToLua(lua_State* l, double x, double y, double z);
		static double LuaGetNumber(lua_State* l, const char* value, int tableIndex);
	};

}
