#include "../pch.h"
#include "Component.h"

void Doom::Component::RemoveComponent()
{
	if (this == nullptr)
		return;
	if (m_Id < m_Owner->GetComponentManager()->m_Components.size())
		m_Owner->GetComponentManager()->m_Components.erase(m_Owner->GetComponentManager()->m_Components.begin() + m_Id);
	unsigned int _size = m_Owner->GetComponentManager()->m_Components.size();
	if (m_Id != _size) {
		for (unsigned int i = m_Id; i < _size; i++)
		{
			m_Owner->GetComponentManager()->m_Components[i]->m_Id = i;
		}
	}
	delete this;
}
