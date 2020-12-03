#include "pch.h"
#include "LuaState.h"

bool Doom::LuaState::CL(int error)
{
	if (error) {
		std::cout << RED "<" << NAMECOLOR << m_FilePath << RED << "> LUA::ERROR::STATE::" << lua_tostring(m_L, -1) << "\n" << RESET;
		return false;
	}
	return true;
}

bool Doom::LuaState::CL(LuaState* l, int error)
{
	if (error) {
		std::cout << RED "<" << NAMECOLOR << l->m_FilePath << RED << "> LUA::ERROR::STATE::" << lua_tostring(l->m_L, -1) << "\n" << RESET;
		return false;
	}
	return true;
}

void Doom::LuaState::RegisterFunction(LuaState* l)
{
	lua_pushlightuserdata(l->m_L, l->m_Go);
	lua_setglobal(l->m_L, GetLuaGlobalName(l->m_Go));

	lua_pushcfunction(l->m_L,LuaGetPosition);
	lua_setglobal(l->m_L , "cpp_GetPosition");
	lua_pushcfunction(l->m_L, LuaTranslate);
	lua_setglobal(l->m_L, "cpp_Translate");

	lua_pushcfunction(l->m_L, LuaGetScale);
	lua_setglobal(l->m_L, "cpp_GetScale");
	lua_pushcfunction(l->m_L, LuaScale);
	lua_setglobal(l->m_L, "cpp_Scale");
}

int Doom::LuaState::LuaGetPosition(lua_State* l)
{
	lua_getglobal(l, GetLuaGlobalName(GetLuaOwner(l)->m_Go));
	GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -1));
	glm::vec3 pos = go->GetPosition();
	lua_pushnumber(l, pos.x);
	lua_pushnumber(l, pos.y);
	lua_pushnumber(l, pos.z);
	return 3;
}

int Doom::LuaState::LuaGetScale(lua_State* l)
{
	lua_getglobal(l, GetLuaGlobalName(GetLuaOwner(l)->m_Go));
	GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -1));
	glm::vec3 scale = go->GetScale();
	lua_pushnumber(l, scale.x);
	lua_pushnumber(l, scale.y);
	lua_pushnumber(l, scale.z);
	return 3;
}

int Doom::LuaState::LuaScale(lua_State* l)
{
	lua_getglobal(l, GetLuaGlobalName(GetLuaOwner(l)->m_Go));
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
	lua_getglobal(l, GetLuaGlobalName(GetLuaOwner(l)->m_Go));
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
	lua_getglobal(m_L,"OnUpdate");
	if(lua_isfunction(m_L,-1)){
		lua_pushnumber(m_L, DeltaTime::m_Deltatime);
		CL(lua_pcall(m_L,1,0,0));
	}
}

void Doom::LuaState::OnStart()
{
	lua_getglobal(m_L, "OnStart");
	CL(lua_pcall(m_L, 0, 0, 0));
}

Doom::LuaState::LuaState(const char* filePath)
{
	m_L = luaL_newstate();
	luaL_openlibs(m_L);
	CL(luaL_dofile(m_L, filePath));
	luaStates.push_back(this);
}

Doom::LuaState::~LuaState()
{
	lua_close(m_L);
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
		return (luaState->m_L == l);
		});
	if (iter != luaStates.end()) {
		LuaState* l = *iter;
		return l;
	}
	return nullptr;
}
