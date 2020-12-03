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

		GameObject* m_Owner = nullptr;

		inline void SetType(const size_t type)  { m_Type = type; }
	public:

		inline size_t GetComponentType() const { return m_Type; }
		inline GameObject* GetOwnerOfComponent() const { return m_Owner; }
		inline int GetComponentID() const { return m_Id; }
		void RemoveComponent();

		Component() {}
		~Component() {}

		friend class ComponentManager;
	};

}

#endif // !COMPONENT_H