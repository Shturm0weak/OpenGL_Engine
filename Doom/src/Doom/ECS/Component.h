#pragma once

#include "../Core/Core.h"
#include "../Core/Listener.h"

namespace Doom {

	class GameObject;

	class DOOM_API Component : public Listener {
	public:

		std::string m_Type;
		GameObject* m_OwnerOfCom = nullptr;
		size_t m_Id = 0;

		template <typename T>
		T* GetComponent()
		{
			return m_OwnerOfCom->m_ComponentManager.GetComponent<T>();
		}

		template<typename T>
		T* AddComponent()
		{
			return m_OwnerOfCom->m_ComponentManager.AddComponent<T>();
		}

		template<typename T>
		void RemoveComponent()
		{
			m_OwnerOfCom->m_ComponentManager.RemoveComponent<T>();
		}

		void RemoveComponent();
		
		virtual void Delete() { delete this; }

		Component() {}
		virtual ~Component();

		friend class ComponentManager;
	};

}