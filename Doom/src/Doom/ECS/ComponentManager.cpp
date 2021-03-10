#include "pch.h"
#include "ComponentManager.h"

using namespace Doom;

template<>
Transform* ComponentManager::GetComponent<Transform>()
{
	return &(m_Owner->m_Transform);
}