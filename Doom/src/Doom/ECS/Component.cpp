#include "../pch.h"
#include "Component.h"

void Doom::Component::RemoveComponent()
{
	if (this == nullptr)
		return;
	if (m_Id < owner->GetComponentManager()->components.size())
		owner->GetComponentManager()->components.erase(owner->GetComponentManager()->components.begin() + m_Id);
	unsigned int _size = owner->GetComponentManager()->components.size();
	if (m_Id != _size) {
		for (unsigned int i = m_Id; i < _size; i++)
		{
			owner->GetComponentManager()->components[i]->m_Id = i;;
		}
	}
	delete this;
}
