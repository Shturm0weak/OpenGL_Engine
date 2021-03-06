#include "pch.h"
#include "LuaState.h"
#include <new>
#include "LuaVec3.h"

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

int Doom::LuaState::LuaGetColor(lua_State* l)
{
	GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -1));
	glm::vec4* color = nullptr;
	Renderer3D* r3d = go->GetComponent<Renderer3D>();
	SpriteRenderer* sr = nullptr;
	if (r3d == nullptr)
		sr = go->GetComponent<SpriteRenderer>();
	else
		color = &(r3d->m_Color);
	if (sr != nullptr)
		color = &(sr->m_Color);
	lua_pushnumber(l, (*color)[0]);
	lua_pushnumber(l, (*color)[1]);
	lua_pushnumber(l, (*color)[2]);
	lua_pushnumber(l, (*color)[3]);
	return 4;
}

int Doom::LuaState::LuaSetColor(lua_State* l)
{
	GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -5));
	Irenderer* ir = nullptr;
	Renderer3D* r3d = go->GetComponent<Renderer3D>();
	SpriteRenderer* sr = nullptr;
	if (r3d == nullptr)
		sr = go->GetComponent<SpriteRenderer>();
	else
		ir = r3d;
	if (sr != nullptr)
		ir = sr;
	if (ir != nullptr)
	{
		ir->m_Color[0] = lua_tonumber(l, -4);
		ir->m_Color[1] = lua_tonumber(l, -3);
		ir->m_Color[2] = lua_tonumber(l, -2);
		ir->m_Color[3] = lua_tonumber(l, -1);
	}
	return 0;
}

void Doom::LuaState::RegisterTransformFunctions(LuaState* l)
{
	lua_newtable(l->m_L);
	int tableIndex = lua_gettop(l->m_L);
	lua_pushvalue(l->m_L, tableIndex);
	lua_setglobal(l->m_L, "Transform");

	lua_pushcfunction(l->m_L, LuaGetTransformPosition);
	lua_setfield(l->m_L, -2, "GetPosition");
	lua_pushcfunction(l->m_L, LuaTransformTranslate);
	lua_setfield(l->m_L, -2, "Translate");

	lua_pushcfunction(l->m_L, LuaGetTransformScale);
	lua_setfield(l->m_L, -2, "GetScale");
	lua_pushcfunction(l->m_L, LuaTransformScale);
	lua_setfield(l->m_L, -2, "Scale");

	lua_pushcfunction(l->m_L, LuaGetTransformRotation);
	lua_setfield(l->m_L, -2, "GetRotation");
	lua_pushcfunction(l->m_L, LuaTransformRotate);
	lua_setfield(l->m_L, -2, "Rotate");

	luaL_newmetatable(l->m_L, "Doom::TransformMetaTable");

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
	RegisterInputFunctions(l);
	RegisterCameraFunctions(l);
	LuaVec3::RegisterLuaVec3Functions(l);
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
	lua_pushcfunction(l->m_L, LuaFindGameObjectById);
	lua_setfield(l->m_L, -2, "FindGameObjectById");
	lua_pushcfunction(l->m_L, LuaGetColor);
	lua_setfield(l->m_L, -2, "GetColor");
	lua_pushcfunction(l->m_L, LuaSetColor);
	lua_setfield(l->m_L, -2, "SetColor");
	lua_pushcfunction(l->m_L, LuaDestroyGameObject);
	lua_setfield(l->m_L, -2, "Delete");

	luaL_newmetatable(l->m_L, "GameObjectMetaTable");
	lua_pushstring(l->m_L, "__gc");
	lua_pushcfunction(l->m_L, LuaGameObjectDestructor);
	lua_settable(l->m_L, -3);

	lua_pushstring(l->m_L, "__index");
	lua_pushcfunction(l->m_L, LuaGetGameObjectProperty);
	lua_settable(l->m_L, -3);

}

int Doom::LuaState::LuaGameObjectDestructor(lua_State* l)
{
	return 0;
}

int Doom::LuaState::LuaGetTransformPosition(lua_State* l)
{
	Transform* tr = static_cast<Transform*>(lua_touserdata(l, -1));
	glm::vec3 position = tr->GetPosition();
	LuaVec3::PushVec3ToLua(l, position.x, position.y, position.z);
	return 1;
}

int Doom::LuaState::LuaGetTransformScale(lua_State* l)
{
	Transform* tr = static_cast<Transform*>(lua_touserdata(l, -1));
	glm::vec3 scale = tr->GetScale();
	LuaVec3::PushVec3ToLua(l, scale.x, scale.y, scale.z);
	return 1;
}

int Doom::LuaState::LuaTransformScale(lua_State* l)
{
	Transform* tr = nullptr; 
	glm::vec3 scale;
	if (lua_istable(l, -1)) {
		tr = static_cast<Transform*>(lua_touserdata(l, -2));
		scale.x = LuaVec3::LuaGetNumber(l, "x", -2);
		scale.y = LuaVec3::LuaGetNumber(l, "y", -2);
		scale.z = LuaVec3::LuaGetNumber(l, "z", -2);
	}
	else {
		tr = static_cast<Transform*>(lua_touserdata(l, -4));
		scale.x = lua_tonumber(l, -3);
		scale.y = lua_tonumber(l, -2);
		scale.z = lua_tonumber(l, -1);
	}
	tr->Scale(scale.x, scale.y, scale.z);
	return 0;
}

void Doom::LuaState::RegisterInputFunctions(LuaState* l)
{
	lua_newtable(l->m_L);
	int tableIndex = lua_gettop(l->m_L);
	lua_pushvalue(l->m_L, tableIndex);
	lua_setglobal(l->m_L, "Input");

	lua_pushcfunction(l->m_L, LuaIsMousePressed);
	lua_setfield(l->m_L, -2, "IsMousePressed");
	lua_pushcfunction(l->m_L, LuaIsMouseReleased);
	lua_setfield(l->m_L, -2, "IsMouseReleased");
	lua_pushcfunction(l->m_L, LuaIsMouseDown);
	lua_setfield(l->m_L, -2, "IsMouseDown");
	lua_pushcfunction(l->m_L, LuaIsKeyDown);
	lua_setfield(l->m_L, -2, "IsKeyDown");
	lua_pushcfunction(l->m_L, LuaIsKeyPressed);
	lua_setfield(l->m_L, -2, "IsKeyPressed");


	luaL_newmetatable(l->m_L, "inputMetaTable");
	lua_settable(l->m_L, -3);
}

int Doom::LuaState::LuaIsMousePressed(lua_State* l)
{
	int32_t key = -1;
	if (lua_isnumber(l, -1))
		key = lua_tonumber(l, -1);
	if (Input::IsMousePressed(key))
		lua_pushboolean(l, true);
	else
		lua_pushboolean(l, false);
	return 1;
}

int Doom::LuaState::LuaIsMouseDown(lua_State* l)
{
	int32_t key = -1;
	if (lua_isnumber(l, -1))
		key = lua_tonumber(l, -1);
	if (Input::IsMouseDown(key))
		lua_pushboolean(l, true);
	else
		lua_pushboolean(l, false);
	return 1;
}

int Doom::LuaState::LuaIsMouseReleased(lua_State* l)
{
	int32_t key = -1;
	if (lua_isnumber(l, -1))
		key = lua_tonumber(l, -1);
	if (Input::IsMouseReleased(key))
		lua_pushboolean(l, true);
	else
		lua_pushboolean(l, false);
	return 1;
}

int Doom::LuaState::LuaIsKeyDown(lua_State* l)
{
	int32_t key = -1;
	if (lua_isnumber(l, -1))
		key = lua_tonumber(l, -1);
	if (Input::IsKeyDown(key))
		lua_pushboolean(l, true);
	else
		lua_pushboolean(l, false);
	return 1;
}

int Doom::LuaState::LuaIsKeyPressed(lua_State* l)
{
	int32_t key = -1;
	if (lua_isnumber(l, -1))
		key = lua_tonumber(l, -1);
	if (Input::IsKeyPressed(key))
		lua_pushboolean(l, true);
	else
		lua_pushboolean(l, false);
	return 1;
}

void Doom::LuaState::RegisterCameraFunctions(LuaState* l)
{
	lua_newtable(l->m_L);
	int tableIndex = lua_gettop(l->m_L);
	lua_pushvalue(l->m_L, tableIndex);
	lua_setglobal(l->m_L, "Camera");

	lua_pushcfunction(l->m_L, LuaGetCameraPosition);
	lua_setfield(l->m_L, -2, "GetPosition");
	lua_pushcfunction(l->m_L, LuaSetCameraPosition);
	lua_setfield(l->m_L, -2, "SetPosition");
	lua_pushcfunction(l->m_L, LuaGetCameraForwardV);
	lua_setfield(l->m_L, -2, "GetForwardVector");
	lua_pushcfunction(l->m_L, LuaGetCameraMouseDirV);
	lua_setfield(l->m_L, -2, "GetMouseDirVector");

	luaL_newmetatable(l->m_L, "CameraMetaTable");
	lua_settable(l->m_L, -3);
}

int Doom::LuaState::LuaGetCameraProperty(lua_State* l)
{
	if (lua_isstring(l, -1) && lua_isuserdata(l, -2))
	{
		Camera* camera = static_cast<Camera*>(lua_touserdata(l, -2));
		const char* index = lua_tostring(l, -1);
		lua_getglobal(l, "Camera");
		lua_pushstring(l, index);
		lua_rawget(l, -2);
		return 1;
	}
	return 0;
}

int Doom::LuaState::LuaGetCameraPosition(lua_State* l)
{
	Camera& camera = Window::GetInstance().GetCamera();
	glm::vec3 position = camera.GetPosition();
	LuaVec3::PushVec3ToLua(l, position.x, position.y, position.z);
	return 1;
}

int Doom::LuaState::LuaSetCameraPosition(lua_State* l)
{
	glm::vec3 position;
	if (lua_istable(l, -1)) {
		position.x = LuaVec3::LuaGetNumber(l, "x", -2);
		position.y = LuaVec3::LuaGetNumber(l, "y", -2);
		position.z = LuaVec3::LuaGetNumber(l, "z", -2);
	}
	else {
		position.x = lua_tonumber(l, -3);
		position.y = lua_tonumber(l, -2);
		position.z = lua_tonumber(l, -1);
	}
	Window::GetInstance().GetCamera().SetPosition(position);
	return 0;
}

int Doom::LuaState::LuaGetCameraRotation(lua_State* l)
{
	Camera& camera = Window::GetInstance().GetCamera();
	glm::vec3 rotation = camera.GetRotation();
	LuaVec3::PushVec3ToLua(l, rotation.x, rotation.y, rotation.z);
	return 1;
}

int Doom::LuaState::LuaSetCameraRotation(lua_State* l)
{
	glm::vec3 rotation;
	if (lua_istable(l, -1)) {
		rotation.x = LuaVec3::LuaGetNumber(l, "x", -2);
		rotation.y = LuaVec3::LuaGetNumber(l, "y", -2);
		rotation.z = LuaVec3::LuaGetNumber(l, "z", -2);
	}
	else {
		rotation.x = lua_tonumber(l, -3);
		rotation.y = lua_tonumber(l, -2);
		rotation.z = lua_tonumber(l, -1);
	}
	Window::GetInstance().GetCamera().SetRotation(rotation);
	return 0;
}

int Doom::LuaState::LuaGetCameraForwardV(lua_State* l)
{
	Camera& camera = Window::GetInstance().GetCamera();
	glm::vec3 forwardV = camera.GetForwardV();
	LuaVec3::PushVec3ToLua(l, forwardV.x, forwardV.y, forwardV.z);
	return 1;
}

int Doom::LuaState::LuaGetCameraMouseDirV(lua_State* l)
{
	Camera& camera = Window::GetInstance().GetCamera();
	glm::vec3 mouseDirV = camera.GetMouseDirVec();
	LuaVec3::PushVec3ToLua(l, mouseDirV.x, mouseDirV.y, mouseDirV.z);
	return 1;
}

int Doom::LuaState::LuaTransformTranslate(lua_State* l)
{
	Transform* tr = nullptr;
	glm::vec3 position;
	if (lua_istable(l, -1)) {
		tr = static_cast<Transform*>(lua_touserdata(l, -2));
		position.x = LuaVec3::LuaGetNumber(l, "x", -2);
		position.y = LuaVec3::LuaGetNumber(l, "y", -2);
		position.z = LuaVec3::LuaGetNumber(l, "z", -2);
	}
	else {
		tr = static_cast<Transform*>(lua_touserdata(l, -4));
		position.x = lua_tonumber(l, -3);
		position.y = lua_tonumber(l, -2);
		position.z = lua_tonumber(l, -1);
	}
	tr->Translate(position);
	return 0;
}

int Doom::LuaState::LuaGetTransformRotation(lua_State* l)
{
	Transform* tr = static_cast<Transform*>(lua_touserdata(l, -1));
	glm::vec3 rotation = tr->GetRotation();
	LuaVec3::PushVec3ToLua(l, rotation.x, rotation.y, rotation.z);
	return 1;
}

int Doom::LuaState::LuaTransformRotate(lua_State* l)
{
	Transform* tr = static_cast<Transform*>(lua_touserdata(l, -5));
	glm::vec3 rotation;
	rotation.x = lua_tonumber(l, -4);
	rotation.y = lua_tonumber(l, -3);
	rotation.z = lua_tonumber(l, -2);
	bool isRad = lua_toboolean(l, -1);
	tr->RotateOnce(rotation, isRad);
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
		if (strcmp(index, "tag") == 0)
		{
			lua_pushstring(l, go->m_Tag.c_str());
			return 1;
		}
		else if (strcmp(index, "id") == 0)
		{
			lua_pushnumber(l, go->m_Id);
			return 1;
		}
		if (strcmp(index, "owner") == 0)
		{
			lua_pushlightuserdata(l, go->GetOwner());
			luaL_getmetatable(l, "GameObjectMetaTable");
			if (lua_istable(l, -1))
				lua_setmetatable(l, -2);
			return 1;
		}
		else if (strcmp(index, "transform") == 0) 
		{
			Transform* tr = go->GetComponent<Transform>();
			lua_pushlightuserdata(l, (void*)tr);
			luaL_getmetatable(l, "Doom::TransformMetaTable");
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

int Doom::LuaState::LuaFindGameObjectById(lua_State* l)
{
	int64_t id = -1;
	if (lua_isnumber(l, -1))
	{
		id = lua_tonumber(l, -1);
		if (id < World::GetInstance().s_GameObjects.size() && id > -1)
		{
			lua_pushlightuserdata(l, World::GetInstance().s_GameObjects[id]);
			luaL_getmetatable(l, "GameObjectMetaTable");
			lua_setmetatable(l, -2);
			return 1;
		}
	}
	return 0;
}

int Doom::LuaState::LuaGetComponent(lua_State* l)
{
	GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -2));
	std::string type = "";
	if (lua_isstring(l, -1)) {
		type = lua_tostring(l, -1);
		Component* component = go->m_ComponentManager.GetComponent(type);
		lua_pushlightuserdata(l, (void*)component);
		luaL_getmetatable(l, type.append("MetaTable").c_str());
		lua_setmetatable(l, -2);
		return 1;
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
	GameObject* go = GameObject::Create(name);
	lua_pushlightuserdata(l, (void*)go);
	luaL_getmetatable(l, "GameObjectMetaTable");
	lua_setmetatable(l, -2);
	return 1;
}

int Doom::LuaState::LuaDestroyGameObject(lua_State* l)
{
	int64_t id = -1;
	if (lua_isnumber(l, -1))
	{
		id = lua_tonumber(l, -1);
		if (id < World::GetInstance().s_GameObjects.size() && id > -1)
		{
			World::GetInstance().DeleteObject(id);
		}
	}
	return 0;
}

//int Doom::LuaState::LuaGetTransform(lua_State* l)
//{
//	GameObject* go = static_cast<GameObject*>(lua_touserdata(l, -1));
//	Transform* tr = go->GetComponent<Transform>();
//	lua_pushlightuserdata(l, (void*)tr);
//	luaL_getmetatable(l, "Doom::TransformMetaTable");
//	if (lua_istable(l, -1))
//		lua_setmetatable(l, -2);
//	return 1;
//}

void Doom::LuaState::OnUpdate(float dt)
{
	int index = lua_getglobal(m_L, "OnUpdate");
	if(lua_isfunction(m_L, index))
	{
		lua_pushnumber(m_L, DeltaTime::s_Deltatime);
		CL(lua_pcall(m_L, 1, 0, 0));
	}
}

void Doom::LuaState::OnStart()
{
	int index = lua_getglobal(m_L, "OnStart");
	if (lua_isfunction(m_L, index))
	{
		CL(lua_pcall(m_L, 0, 0, 0));
	}
}

Doom::LuaState::LuaState(const char* filePath)
{
	m_FilePath = filePath;
	m_L = luaL_newstate();
	luaL_openlibs(m_L);
	s_LuaStates.push_back(this);
}

Doom::LuaState::~LuaState()
{
	lua_close(m_L);
	auto iter = std::find(s_LuaStates.begin(), s_LuaStates.end(), this);
	if (iter != s_LuaStates.end())
		s_LuaStates.erase(iter);
#ifdef _DEBUG
	std::cout << "Destroy LuaState\n";
#endif
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