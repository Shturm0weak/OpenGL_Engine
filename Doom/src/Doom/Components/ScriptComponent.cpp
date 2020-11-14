#include "pch.h"
#include "ScriptComponent.h"
#include "Lua/LuaState.h"

void Doom::ScriptComponent::AssignScript(const char* filePath)
{
	lState = new LuaState(filePath);
	lState->go = GetOwnerOfComponent();
	LuaState::RegisterFunction(lState);
}

Doom::ScriptComponent::ScriptComponent()
{
	//SetType(ComponentType::SCRIPT);
}

Doom::ScriptComponent::~ScriptComponent()
{
	if (lState != nullptr) {
		delete lState;
	}
}
