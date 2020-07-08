#ifndef COMPONENT_H
#define COMPONENT_H

#include "../Core/Core.h"

namespace Doom {

	class GameObject;

	class DOOM_API Component {
	private:
		int m_Id;
		const char* m_Type;
	protected:
		GameObject* owner = nullptr;
		inline void SetType(const char* type)  { m_Type = type; }
		inline int GetComponentId() const { return m_Id; }
		inline const char* GetComponentType() const { return m_Type; }
	public:
		inline GameObject* GetOwnerOfComponent() const { return owner; }
		inline int GetComponentID() const { return m_Id; }
		
		friend class ComponentManager;

		void RemoveComponent();

		~Component() {}
	};

}

#endif // !COMPONENT_H