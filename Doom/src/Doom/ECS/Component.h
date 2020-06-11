#ifndef COMPONENT_H
#define COMPONENT_H
#include "IComponent.h"

namespace Doom {

	class GameObject;

	class DOOM_API Component : private IComponent {
	private:
		int m_Id;
		const char* m_Type;
	protected:
		GameObject* owner = nullptr;

		virtual void SetType(const char* type) override { m_Type = type; }
		virtual int GetComponentId() override { return m_Id; }
		virtual const char* GetComponentType() override { return m_Type; }
	public:
		GameObject& GetOwnerOfComponent() { return *owner; }
		int GetComponentID() { return m_Id; }
		friend class ComponentManager;

		~Component() {}
	};

}

#endif // !COMPONENT_H