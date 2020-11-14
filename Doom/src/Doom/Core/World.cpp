#include "../pch.h"
#include "World.h"
#include "Lua/LuaState.h"

void Doom::World::ProccessLuaStates()
{
	for(auto l : LuaState::luaStates)
	{
		l->OnUpdate(DeltaTime::deltatime);
	}
}

void Doom::World::StartLuaStates()
{
	for (auto l : LuaState::luaStates)
	{
		l->OnStart();
	}
}
