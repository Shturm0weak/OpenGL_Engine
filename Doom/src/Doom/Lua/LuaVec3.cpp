#include "pch.h"
#include "LuaVec3.h"
#include "LuaState.h"

double Doom::LuaVec3::LuaGetNumber(lua_State* l, const char* value, int tableIndex)
{
	lua_pushstring(l, value);
	lua_gettable(l, tableIndex);
	double number = lua_tonumber(l, -1);
	lua_pop(l, 1);
	return number;
}

void Doom::LuaVec3::RegisterLuaVec3Functions(LuaState* l)
{
	//lua_newtable(l->m_L);
	//int tableIndex = lua_gettop(l->m_L);
	//lua_pushvalue(l->m_L, tableIndex);
	//lua_setglobal(l->m_L, "vec3");

	lua_pushcfunction(l->m_L, Vec3);
	lua_setglobal(l->m_L, "newVec3");

	luaL_newmetatable(l->m_L, "Vec3MetaTable");
	lua_pushstring(l->m_L, "__add");
	lua_pushcfunction(l->m_L, Add);
	lua_settable(l->m_L, -3);
	lua_pushstring(l->m_L, "__sub");
	lua_pushcfunction(l->m_L, Sub);
	lua_settable(l->m_L, -3);
	lua_pushstring(l->m_L, "__mul");
	lua_pushcfunction(l->m_L, Mul);
	lua_settable(l->m_L, -3);

	/*lua_pushstring(l->m_L, "__gc");
	lua_pushcfunction(l->m_L, LuaGameObjectDestructor);
	lua_settable(l->m_L, -3);

	lua_pushstring(l->m_L, "__index");
	lua_pushcfunction(l->m_L, LuaGetGameObjectProperty);
	lua_settable(l->m_L, -3);*/
}

int Doom::LuaVec3::Vec3(lua_State* l)
{
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	if(lua_isnumber(l, -3))
		x = lua_tonumber(l, -3);
	if (lua_isnumber(l, -2))
		y = lua_tonumber(l, -2);
	if (lua_isnumber(l, -1))
		z = lua_tonumber(l, -1);

	lua_newtable(l);
	lua_pushstring(l, "x");
	lua_pushnumber(l, x);
	lua_settable(l, -3);

	lua_pushstring(l, "y");
	lua_pushnumber(l, y);
	lua_settable(l, -3);

	lua_pushstring(l, "z");
	lua_pushnumber(l, z);
	lua_settable(l, -3);

	luaL_getmetatable(l, "Vec3MetaTable");
	lua_setmetatable(l, -2);
	return 1;
}

int Doom::LuaVec3::Add(lua_State* l)
{
	if (lua_istable(l, -1) && lua_istable(l, -2)) {
		
		//x
		double xLeft = LuaGetNumber(l, "x", -3);
		double xRight = LuaGetNumber(l, "x", -2);
		double xAdded = xLeft + xRight;
		
		//y
		double yLeft = LuaGetNumber(l, "y", -3);
		double yRight = LuaGetNumber(l, "y", -2);
		double yAdded = yLeft + yRight;

		//z
		double zLeft = LuaGetNumber(l, "z", -3);
		double zRight = LuaGetNumber(l, "z", -2);
		double zAdded = zLeft + zRight;

		PushVec3ToLua(l, xAdded ,yAdded, zAdded);
		return 1;
	}
	else if (lua_istable(l, -1) && lua_isnumber(l, -2)) {

		double number = lua_tonumber(l, -2);
		double xLeft = LuaGetNumber(l, "x", -2);
		double yLeft = LuaGetNumber(l, "y", -2);
		double zLeft = LuaGetNumber(l, "z", -2);

		PushVec3ToLua(l, xLeft + number, yLeft + number, zLeft + number);
		return 1;
	}
	else if (lua_istable(l, -2) && lua_isnumber(l, -1)) {

		double number = lua_tonumber(l, -1);
		double xLeft = LuaGetNumber(l, "x", -3);
		double yLeft = LuaGetNumber(l, "y", -3);
		double zLeft = LuaGetNumber(l, "z", -3);

		PushVec3ToLua(l, xLeft + number, yLeft + number, zLeft + number);
		return 1;
	}
	return 0;
}

int Doom::LuaVec3::Sub(lua_State* l)
{
	if (lua_istable(l, -1) && lua_istable(l, -2)) {

		//x
		double xLeft = LuaGetNumber(l, "x", -3);
		double xRight = LuaGetNumber(l, "x", -2);
		double xAdded = xLeft - xRight;

		//y
		double yLeft = LuaGetNumber(l, "y", -3);
		double yRight = LuaGetNumber(l, "y", -2);
		double yAdded = yLeft - yRight;

		//z
		double zLeft = LuaGetNumber(l, "z", -3);
		double zRight = LuaGetNumber(l, "z", -2);
		double zAdded = zLeft - zRight;

		PushVec3ToLua(l, xAdded, yAdded, zAdded);
		return 1;
	}
	else if (lua_istable(l, -1) && lua_isnumber(l, -2)) {

		double number = lua_tonumber(l, -2);
		double xLeft = LuaGetNumber(l, "x", -2);
		double yLeft = LuaGetNumber(l, "y", -2);
		double zLeft = LuaGetNumber(l, "z", -2);

		PushVec3ToLua(l, xLeft - number, yLeft - number, zLeft - number);
		return 1;
	}
	else if (lua_istable(l, -2) && lua_isnumber(l, -1)) {

		double number = lua_tonumber(l, -1);
		double xLeft = LuaGetNumber(l, "x", -3);
		double yLeft = LuaGetNumber(l, "y", -3);
		double zLeft = LuaGetNumber(l, "z", -3);

		PushVec3ToLua(l, xLeft - number, yLeft - number, zLeft - number);
		return 1;
	}
	return 0;
}

int Doom::LuaVec3::Mul(lua_State* l)
{
	if (lua_istable(l, -1) && lua_istable(l, -2)) {

		//x
		double xLeft = LuaGetNumber(l, "x", -3);
		double xRight = LuaGetNumber(l, "x", -2);
		double xAdded = xLeft - xRight;

		//y
		double yLeft = LuaGetNumber(l, "y", -3);
		double yRight = LuaGetNumber(l, "y", -2);
		double yAdded = yLeft - yRight;

		//z
		double zLeft = LuaGetNumber(l, "z", -3);
		double zRight = LuaGetNumber(l, "z", -2);
		double zAdded = zLeft - zRight;

		lua_pushnumber(l, xAdded + yAdded + zAdded);
		return 1;
	}
	else if (lua_istable(l, -1) && lua_isnumber(l, -2)) {

		double number = lua_tonumber(l, -2);
		double xLeft = LuaGetNumber(l, "x", -2);
		double yLeft = LuaGetNumber(l, "y", -2);
		double zLeft = LuaGetNumber(l, "z", -2);

		PushVec3ToLua(l, xLeft * number, yLeft * number, zLeft * number);
		return 1;
	}
	else if (lua_istable(l, -2) && lua_isnumber(l, -1)) {

		double number = lua_tonumber(l, -1);
		double xLeft = LuaGetNumber(l, "x", -3);
		double yLeft = LuaGetNumber(l, "y", -3);
		double zLeft = LuaGetNumber(l, "z", -3);

		PushVec3ToLua(l, xLeft * number, yLeft * number, zLeft * number);
		return 1;
	}
	return 0;
}

void Doom::LuaVec3::PushVec3ToLua(lua_State* l, double x, double y, double z)
{
	Vec3(l);
	lua_pushstring(l, "x");
	lua_pushnumber(l, x);
	lua_rawset(l, -3);
	lua_pushstring(l, "y");
	lua_pushnumber(l, y);
	lua_rawset(l, -3);
	lua_pushstring(l, "z");
	lua_pushnumber(l, z);
	lua_rawset(l, -3);
}