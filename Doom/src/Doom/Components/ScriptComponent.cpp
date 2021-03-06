#include "pch.h"
#include "ScriptComponent.h"
#include "Lua/LuaState.h"

void Doom::ScriptComponent::AssignScript(const char* filePath)
{
	if (m_LState != nullptr) 
	{
		delete m_LState;
		m_LState = nullptr;
	}
	m_LState = new LuaState(filePath);
	m_LState->m_Go = GetOwnerOfComponent();
	LuaState::RegisterFunction(m_LState);
	m_LState->CL(luaL_dofile(m_LState->m_L, filePath));
	std::function<void()>* f = new std::function<void()>([=] { m_LState->OnStart(); });  //Nice method to call any function/code on the next frame
	EventSystem::GetInstance().SendEvent(EventType::ONMAINTHREADPROCESS, nullptr, f);    //Temporary solution until I implement start button in the Editor 
}

Doom::Component* Doom::ScriptComponent::Create()
{
	return new ScriptComponent();
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
	if (m_LState != nullptr)
		delete m_LState;
}
