#pragma once
#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Component.h"
#include "../Render/Renderer.h"
#include "../Components/SpriteRenderer.h"
#include "../Components/Render3D.h"
#include "../Components/Transform.h"
#include "../Components/Animator.h"
#include "../Components/RectangleCollider2D.h"
#include "../Components/CubeCollider.h"
#include "../Enums/ColoredOutput.h"
#include "../Components/PointLight.h"
#include "../Components/DirectionalLight.h"
#include "../Components/SphereCollider.h"
#include "../Components/ScriptComponent.h"

namespace Doom {

	class DOOM_API ComponentManager {
	private:

		GameObject* m_Owner = nullptr;
		std::string m_OwnerName;
		const char** m_NamesOfMembers = nullptr;
		std::vector <Component*> m_Components;

		friend class Component;

	public:

		ComponentManager::ComponentManager(GameObject* owner, std::string& owner_name) {
			this->m_Owner = owner;
			this->m_OwnerName = owner_name;
		}

		inline int GetAmountOfComponents() { return m_Components.size(); }

		~ComponentManager() {
			delete[] m_NamesOfMembers;
			RemoveComponent<RectangleCollider2D>();
			RemoveComponent<Animator>();
			RemoveComponent<CubeCollider3D>();
			RemoveComponent<PointLight>();
			RemoveComponent<DirectionalLight>();
			RemoveComponent<Renderer3D>();
			RemoveComponent<SpriteRenderer>();
			RemoveComponent<SphereCollider>();
			RemoveComponent<Transform>();

			std::vector<ScriptComponent*> scripts = GetScripts();
			for (uint32_t i = 0; i < scripts.size(); i++)
			{
				RemoveComponent(scripts[i]);
			}
		}

		const char** GetItems() {
			if (m_NamesOfMembers != nullptr)
				delete[] m_NamesOfMembers;
			m_NamesOfMembers = new const char* [m_Components.size()];
			for (unsigned int i = 0; i < m_Components.size(); i++)
			{
				m_NamesOfMembers[i] = std::to_string((int)m_Components[i]->GetComponentType()).c_str();
			}
			return m_NamesOfMembers;
		}

		std::vector<ScriptComponent*> GetScripts() {
			std::vector<ScriptComponent*> scripts;
			for (Component* com : m_Components) {
				if (com->m_Type == sizeof(ScriptComponent))
					scripts.push_back(static_cast<ScriptComponent*>(com));
			}
			return scripts;
		}

		template <class T>
		void RemoveComponent() {
			T* com = nullptr;
			com = GetComponent<T>();
			if (com == nullptr)
				return;
			int _id = com->m_Id;
			if (com->m_Id < m_Components.size())
				m_Components.erase(m_Components.begin() + com->m_Id);
			unsigned int _size = m_Components.size();
			if (com->m_Id != _size) {
				for (unsigned int i = com->m_Id; i < _size; i++)
				{
					m_Components[i]->m_Id = i;
				}
			}
			delete com;
			return;
		}

		void RemoveComponent(Component* com) {
			if (com == nullptr)
				return;
			int _id = com->m_Id;
			if (com->m_Id < m_Components.size())
				m_Components.erase(m_Components.begin() + com->m_Id);
			unsigned int _size = m_Components.size();
			if (com->m_Id != _size) {
				for (unsigned int i = com->m_Id; i < _size; i++)
				{
					m_Components[i]->m_Id = i;
				}
			}
			delete com;
			return;
		}

		template<class T>
		T* GetComponent()
		{
			for (auto com : m_Components) {
				if (com->GetComponentType() == sizeof(T)) {
					return static_cast<T*>(com);
				}
			}
			return nullptr;
		}

		template<class T>
		T* AddComponent() {
			if ((GetComponent<T>() != nullptr && GetComponent<T>()->m_Type == sizeof(ScriptComponent)) || GetComponent<T>() == nullptr) {
				T* object = new T();
				object->m_Owner = (this->m_Owner);
				object->SetType(sizeof(T));
				object->m_Id = m_Components.size();
				m_Components.push_back(object);
#ifdef _DEBUG
				
				std::cout << NAMECOLOR << (int)object->m_Type << RESET << ": has been added to GameObject <" NAMECOLOR << m_OwnerName << RESET << ">" << std::endl;
#endif
				return object;
			}
			return nullptr;
		}
	};
}
#endif