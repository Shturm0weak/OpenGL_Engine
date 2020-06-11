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

		 void SetType(const char* type)  { m_Type = type; }
		 int GetComponentId()  { return m_Id; }
		 const char* GetComponentType()  { return m_Type; }
	public:
		GameObject* GetOwnerOfComponent() { return owner; }
		int GetComponentID() { return m_Id; }
		friend class ComponentManager;

		~Component() {}
	};

}

#endif // !COMPONENT_H