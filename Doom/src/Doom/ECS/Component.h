#ifndef COMPONENT_H
#define COMPONENT_H

#include "../Core/Core.h"
#include "../Core/Listener.h"

namespace Doom {

	class GameObject;

	class DOOM_API Component : public Listener {
	private:

		int m_Id;
		std::string m_Type;
	protected:

		GameObject* m_OwnerOfCom = nullptr;
		void SetType(std::string type) { m_Type = type; }
	public:

		template <typename T>
		T* GetComponent() {
			return m_OwnerOfCom->GetComponentManager()->GetComponent<T>();
		}

		template<typename T>
		T* AddComponent() {
			return m_OwnerOfCom->GetComponentManager()->AddComponent<T>();
		}

		template<typename T>
		void RemoveComponent() {
			m_OwnerOfCom->GetComponentManager()->RemoveComponent<T>();
		}

		inline std::string GetComponentType() const { return m_Type; }
		inline GameObject* GetOwnerOfComponent() const { return m_OwnerOfCom; }
		inline int GetComponentID() const { return m_Id; }
		void RemoveComponent();

		Component() {}
		virtual ~Component();

		friend class ComponentManager;
	};

}

#endif // !COMPONENT_H