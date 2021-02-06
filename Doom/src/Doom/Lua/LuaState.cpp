#include "pch.h"
#include "LuaState.h"
#include <new>

bool Doom::LuaState::CL(int error)
{
	if (error)
	{
		std::cout << RED "<" << NAMECOLOR << m_FilePath << RED << "> LUA::ERROR::STATE::" << lua_tostring(m_L, -1) << "\n" << RESET;
		return false;
	}
	return true;
}

bool Doom::LuaState::CL(LuaState* l, int error)
{
	if (error)
	{
		std::cout << RED "<" << NAMECOLOR << l->m_FilePath << RED << "> LUA::ERROR::STATE::" << lua_tostring(l->m_L, -1) << "\n" << RESET;
		return false;
	}
	return true;
}

void Doom::LuaState::RegisterTransformFunctions(LuaState* l)
{
	lua_newtable(l->m_L);
	int tableIndex = lua_gettop(l->m_L);
	lua_pushvalue(l->m_L, tableIndex);
	lua_setglobal(l->m_L, "Transform");

	lua_pushcfunction(l->m_L, LuaGetPosition);
	lua_setfield(l->m_L, -2, "GetPosition");
	lua_pushcfunction(l->m_L, LuaTranslate);
	lua_setfield(l->m_L, -2, "Translate");

	lua_pushcfunction(l->m_L, LuaGetScale);
	lua_setfield(l->m_L, -2, "GetScale");
	lua_pushcfunction(l->m_L, LuaScale);
	lua_setfield(l->m_L, -2, "Scale");

	lua_pushcfunction(l->m_L, LuaGetRotation);
	lua_setfield(l->m_L, -2, "GetRotation");
	lua_pushcfunction(l->m_L, LuaRotate);
	lua_setfield(l->m_L, -2, "Rotate");

	luaL_newmetatable(l->m_L, "TransformMetaTable");

	lua_pushstring(l->m_L, "__index");
	lua_pushcfunction(l->m_L, LuaGetTransformProperty);
	lua_settable(l->m_L, -3);
}

int Doom::LuaState::LuaGetTransformProperty(lua_State* l)
{
	if (lua_isstring(l, -1) && lua_isuserdata(l, -2))
	{
		GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -2));
		const char* index = lua_tostring(l, -1);
		lua_getglobal(l, "Transform");
		lua_pushstring(l, index);
		lua_rawget(l, -2);
		return 1;
	}
	return 0;
}

void Doom::LuaState::RegisterFunction(LuaState* l)
{
	RegisterGameObjectsFunctions(l);
	RegisterTransformFunctions(l);
}

void Doom::LuaState::RegisterGameObjectsFunctions(LuaState* l)
{
	lua_newtable(l->m_L);
	int tableIndex = lua_gettop(l->m_L);
	lua_pushvalue(l->m_L, tableIndex);
	lua_setglobal(l->m_L, "GameObject");

	lua_pushcfunction(l->m_L, LuaGetThis);
	lua_setfield(l->m_L, -2, "This");
	lua_pushcfunction(l->m_L, LuaCreate);
	lua_setfield(l->m_L, -2, "New");
	lua_pushcfunction(l->m_L, LuaGetTransform);
	lua_setfield(l->m_L, -2, "GetTransform");

	luaL_newmetatable(l->m_L, "GameObjectMetaTable");
	lua_pushstring(l->m_L, "__gc");
	lua_pushcfunction(l->m_L, LuaDestroyGameObject);
	lua_settable(l->m_L, -3);

	lua_pushstring(l->m_L, "__index");
	lua_pushcfunction(l->m_L, LuaGetGameObjectProperty);
	lua_settable(l->m_L, -3);

}

int Doom::LuaState::LuaGetPosition(lua_State* l)
{
	Transform* tr = static_cast<Transform*>(lua_touserdata(l, -1));
	glm::vec3 pos = tr->GetPosition();
	lua_pushnumber(l, pos.x);
	lua_pushnumber(l, pos.y);
	lua_pushnumber(l, pos.z);
	return 3;
}

int Doom::LuaState::LuaGetScale(lua_State* l)
{
	Transform* tr = static_cast<Transform*>(lua_touserdata(l, -1));
	glm::vec3 scale = tr->GetScale();
	lua_pushnumber(l, scale.x);
	lua_pushnumber(l, scale.y);
	lua_pushnumber(l, scale.z);
	return 3;
}

int Doom::LuaState::LuaScale(lua_State* l)
{
	Transform* tr = static_cast<Transform*>(lua_touserdata(l, -4));
	glm::vec3 scale;
	scale.x = lua_tonumber(l, -3);
	scale.y = lua_tonumber(l, -2);
	scale.z = lua_tonumber(l, -1);
	tr->Scale(scale.x, scale.y, scale.z);
	return 0;
}

int Doom::LuaState::LuaTranslate(lua_State* l)
{
	Transform* tr = static_cast<Transform*>(lua_touserdata(l, -4));
	glm::vec3 pos;
	pos.x = lua_tonumber(l, -3);
	pos.y = lua_tonumber(l, -2);
	pos.z = lua_tonumber(l, -1);
	tr->Translate(pos.x, pos.y, pos.z);
	return 0;
}

int Doom::LuaState::LuaGetRotation(lua_State* l)
{
	Transform* tr = static_cast<Transform*>(lua_touserdata(l, -1));
	glm::vec3 rotation = tr->GetRotation();
	lua_pushnumber(l, rotation.x);
	lua_pushnumber(l, rotation.y);
	lua_pushnumber(l, rotation.z);
	return 3;
}

int Doom::LuaState::LuaRotate(lua_State* l)
{
	Transform* tr = static_cast<Transform*>(lua_touserdata(l, -5));
	glm::vec3 rot;
	rot.x = lua_tonumber(l, -4);
	rot.y = lua_tonumber(l, -3);
	rot.z = lua_tonumber(l, -2);
	bool isRad = lua_toboolean(l, -1);
	tr->RotateOnce(rot, isRad);
	return 0;
}

int Doom::LuaState::LuaGetGameObjectProperty(lua_State* l)
{
	if (lua_isstring(l, -1) && lua_isuserdata(l, -2))
	{
		GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -2));
		const char* index = lua_tostring(l, -1);
		if (strcmp(index, "name") == 0)
		{
			lua_pushstring(l, go->m_Name.c_str());
			return 1;
		}
		else if (strcmp(index, "transform") == 0) 
		{
			Transform* tr = go->GetComponent<Transform>();
			lua_pushlightuserdata(l, (void*)tr);
			luaL_getmetatable(l, "TransformMetaTable");
			if (lua_istable(l, -1))
				lua_setmetatable(l, -2);
			return 1;
		}
		else
		{
			lua_getglobal(l, "GameObject");
			lua_pushstring(l, index);
			lua_rawget(l, -2);
			return 1;
		}
	}
	return 0;
}

int Doom::LuaState::LuaGetThis(lua_State* l)
{
	GameObject* go = GetLuaOwner(l)->m_Go;
	lua_pushlightuserdata(l, (void*)go);
	luaL_getmetatable(l, "GameObjectMetaTable");
	lua_setmetatable(l, -2);
	return 1;
	//lua_setglobal(l, GetLuaGlobalName(go));
	//void* ptr = lua_newuserdata(l, sizeof(GameObject));
	//new (ptr) GameObject()
}

int Doom::LuaState::LuaCreate(lua_State* l)
{
	std::string name = "Unnamed";
	if (lua_isstring(l, -1))
		name = lua_tostring(l, -1);
	GameObject* go = new GameObject(name);
	lua_pushlightuserdata(l, (void*)go);
	luaL_getmetatable(l, "GameObjectMetaTable");
	lua_setmetatable(l, -2);
	return 1;
}

int Doom::LuaState::LuaDestroyGameObject(lua_State* l)
{
	return 0;
}

int Doom::LuaState::LuaGetTransform(lua_State* l)
{
	GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -1));
	Transform* tr = go->GetComponent<Transform>();
	lua_pushlightuserdata(l, (void*)tr);
	luaL_getmetatable(l, "TransformMetaTable");
	if (lua_istable(l, -1))
		lua_setmetatable(l, -2);
	return 1;
}

void Doom::LuaState::OnUpdate(float dt)
{
	int index = lua_getglobal(m_L, "OnUpdate");
	if(lua_isfunction(m_L, index))
	{
		lua_pushnumber(m_L, DeltaTime::s_Deltatime);
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
	s_LuaStates.push_back(this);
}

Doom::LuaState::~LuaState()
{
	std::cout << "Destroy LuaState\n";
	lua_close(m_L);
	auto iter = std::find(s_LuaStates.begin(), s_LuaStates.end(), this);
	if (iter != s_LuaStates.end())
		s_LuaStates.erase(iter);
}

const char* Doom::LuaState::GetLuaGlobalName(GameObject* go)
{
	std::string name = go->m_Name.c_str();
	name.append(std::to_string(go->m_Id).c_str());
	return name.c_str();
}

Doom::LuaState* Doom::LuaState::GetLuaOwner(lua_State* l)
{
	auto iter = std::find_if(s_LuaStates.begin(), s_LuaStates.end(), [=] (LuaState* luaState) {
		return (luaState->m_L == l);
		});
	if (iter != s_LuaStates.end())
	{
		LuaState* l = *iter;
		return l;
	}
	return nullptr;
}
