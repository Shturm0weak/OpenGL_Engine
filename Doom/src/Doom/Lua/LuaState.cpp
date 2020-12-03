#include "pch.h"
#include "LuaState.h"

bool Doom::LuaState::CL(int error)
{
	if (error) {
		std::cout << RED "<" << NAMECOLOR << filePath << RED << "> LUA::ERROR::STATE::" << lua_tostring(l, -1) << "\n" << RESET;
		return false;
	}
	return true;
}

bool Doom::LuaState::CL(LuaState* l, int error)
{
	if (error) {
		std::cout << RED "<" << NAMECOLOR << l->filePath << RED << "> LUA::ERROR::STATE::" << lua_tostring(l->l, -1) << "\n" << RESET;
		return false;
	}
	return true;
}

void Doom::LuaState::RegisterFunction(LuaState* l)
{
	lua_pushlightuserdata(l->l, l->go);
	lua_setglobal(l->l, GetLuaGlobalName(l->go));

	lua_pushcfunction(l->l,LuaGetPosition);
	lua_setglobal(l->l , "cpp_GetPosition");
	lua_pushcfunction(l->l, LuaTranslate);
	lua_setglobal(l->l, "cpp_Translate");

	lua_pushcfunction(l->l, LuaGetScale);
	lua_setglobal(l->l, "cpp_GetScale");
	lua_pushcfunction(l->l, LuaScale);
	lua_setglobal(l->l, "cpp_Scale");
}

int Doom::LuaState::LuaGetPosition(lua_State* l)
{
	lua_getglobal(l, GetLuaGlobalName(GetLuaOwner(l)->go));
	GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -1));
	glm::vec3 pos = go->GetPosition();
	lua_pushnumber(l, pos.x);
	lua_pushnumber(l, pos.y);
	lua_pushnumber(l, pos.z);
	return 3;
}

int Doom::LuaState::LuaGetScale(lua_State* l)
{
	lua_getglobal(l, GetLuaGlobalName(GetLuaOwner(l)->go));
	GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -1));
	glm::vec3 scale = go->GetScale();
	lua_pushnumber(l, scale.x);
	lua_pushnumber(l, scale.y);
	lua_pushnumber(l, scale.z);
	return 3;
}

int Doom::LuaState::LuaScale(lua_State* l)
{
	lua_getglobal(l, GetLuaGlobalName(GetLuaOwner(l)->go));
	GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -1));
	glm::vec3 scale;
	scale.x = lua_tonumber(l, 3);
	scale.y = lua_tonumber(l, 2);
	scale.z = lua_tonumber(l, 1);
	go->m_Transform->Scale(scale.x, scale.y, scale.z);
	return 0;
}

int Doom::LuaState::LuaTranslate(lua_State* l)
{
	lua_getglobal(l, GetLuaGlobalName(GetLuaOwner(l)->go));
	GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -1));
	glm::vec3 pos;
	pos.x = lua_tonumber(l, 3);
	pos.y = lua_tonumber(l, 2);
	pos.z = lua_tonumber(l, 1);
	go->m_Transform->Translate(pos.x, pos.y, pos.z);
	return 0;
}

void Doom::LuaState::OnUpdate(float dt)
{
	lua_getglobal(l,"OnUpdate");
	if(lua_isfunction(l,-1)){
		lua_pushnumber(l, DeltaTime::m_Deltatime);
		CL(lua_pcall(l,1,0,0));
	}
}

void Doom::LuaState::OnStart()
{
	lua_getglobal(l, "OnStart");
	CL(lua_pcall(l, 0, 0, 0));
}

Doom::LuaState::LuaState(const char* filePath)
{
	l = luaL_newstate();
	luaL_openlibs(l);
	CL(luaL_dofile(l, filePath));
	luaStates.push_back(this);
}

Doom::LuaState::~LuaState()
{
	lua_close(l);
	auto iter = std::find(luaStates.begin(), luaStates.end(), this);
	if (iter != luaStates.end()) {
		luaStates.erase(iter);
	}
}

const char* Doom::LuaState::GetLuaGlobalName(GameObject* go)
{
	std::string name = go->m_Name.c_str();
	name.append(std::to_string(go->m_Id).c_str());
	return name.c_str();
}

Doom::LuaState* Doom::LuaState::GetLuaOwner(lua_State* l)
{
	auto iter = std::find_if(luaStates.begin(), luaStates.end(), [=] (LuaState* luaState) {
		return (luaState->l == l);
		});
	if (iter != luaStates.end()) {
		LuaState* l = *iter;
		return l;
	}
	return nullptr;
}
