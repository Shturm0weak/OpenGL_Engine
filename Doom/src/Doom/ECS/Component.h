#ifndef COMPONENT_H
#define COMPONENT_H

#include "../Core/Core.h"

namespace Doom {

	class GameObject;
	enum class ComponentType;

	class DOOM_API Component {
	private:
		int m_Id;
		size_t m_Type;
	protected:
		GameObject* owner = nullptr;
		inline void SetType(const size_t type)  { m_Type = type; }
		inline int GetComponentId() const { return m_Id; }
	public:
		inline size_t GetComponentType() const { return m_Type; }
		inline GameObject* GetOwnerOfComponent() const { return owner; }
		inline int GetComponentID() const { return m_Id; }
		
		friend class ComponentManager;

		void RemoveComponent();

		~Component() {}
	};

}

#endif // !COMPONENT_H