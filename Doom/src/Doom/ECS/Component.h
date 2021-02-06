#pragma once

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
		T* GetComponent()
		{
			return m_OwnerOfCom->GetComponentManager()->GetComponent<T>();
		}

		template<typename T>
		T* AddComponent()
		{
			return m_OwnerOfCom->GetComponentManager()->AddComponent<T>();
		}

		template<typename T>
		void RemoveComponent()
		{
			m_OwnerOfCom->GetComponentManager()->RemoveComponent<T>();
		}

		std::string GetComponentType() const { return m_Type; }
		GameObject* GetOwnerOfComponent() const { return m_OwnerOfCom; }
		int GetComponentID() const { return m_Id; }
		void RemoveComponent();

		Component() {}
		virtual ~Component();

		friend class ComponentManager;
	};

}