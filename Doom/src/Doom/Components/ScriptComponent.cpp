#include "pch.h"
#include "ScriptComponent.h"
#include "Lua/LuaState.h"

void Doom::ScriptComponent::AssignScript(const char* filePath)
{
	m_LState = new LuaState(filePath);
	m_LState->m_Go = GetOwnerOfComponent();
	LuaState::RegisterFunction(m_LState);
}

Doom::ScriptComponent::ScriptComponent(const ScriptComponent& rhs)
{

}

Doom::ScriptComponent::ScriptComponent()
{
	//SetType(ComponentType::SCRIPT);
}

Doom::ScriptComponent::~ScriptComponent()
{
	if (m_LState != nullptr) {
		delete m_LState;
	}
}
