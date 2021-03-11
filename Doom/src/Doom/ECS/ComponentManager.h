#pragma once

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
#include "../Components/ParticleEmitter.h"
#include "Core/Utils.h"

namespace Doom {

	class DOOM_API ComponentManager {
	private:

		GameObject* m_Owner = nullptr;
		const char** m_NamesOfMembers = nullptr;
		std::vector <Component*> m_Components;

		template<class T>
		void CopyComponent(ComponentManager& rhs) 
		{
			if (rhs.GetComponent<T>() != nullptr)
			{
				AddComponent<T>()->operator=(*rhs.GetComponent<T>());
			}
		}

		void Copy(ComponentManager& rhs) 
		{
			CopyComponent<Transform>(rhs);
			CopyComponent<PointLight>(rhs);
			CopyComponent<DirectionalLight>(rhs);
			CopyComponent<Renderer3D>(rhs);
			CopyComponent<SpriteRenderer>(rhs);
			CopyComponent<SphereCollider>(rhs);
			CopyComponent<Animator>(rhs);
			CopyComponent<ParticleEmitter>(rhs);
		}

		static void ShutDown() {
			for (auto iter = Renderer3D::s_MemoryPool.begin(); iter != Renderer3D::s_MemoryPool.end(); iter++)
			{
				delete[] (Renderer3D*)iter->first;
			}
			Renderer3D::s_FreeMemory.clear();

			for (auto iter = SpriteRenderer::s_MemoryPool.begin(); iter != SpriteRenderer::s_MemoryPool.end(); iter++)
			{
				delete[] (SpriteRenderer*)iter->first;
			}
			SpriteRenderer::s_FreeMemory.clear();

			for (auto iter = RectangleCollider2D::s_MemoryPool.begin(); iter != RectangleCollider2D::s_MemoryPool.end(); iter++)
			{
				delete[] (RectangleCollider2D*)iter->first;
			}
			RectangleCollider2D::s_FreeMemory.clear();

			for (auto iter = CubeCollider3D::s_MemoryPool.begin(); iter != CubeCollider3D::s_MemoryPool.end(); iter++)
			{
				delete[] (CubeCollider3D*)iter->first;
			}
			CubeCollider3D::s_FreeMemory.clear();
		}

		friend class Component;
		friend class Editor;
		friend class EntryPoint;
	public:

		ComponentManager(ComponentManager& rhs) 
		{
			Copy(rhs);
		}

		ComponentManager(GameObject* owner)
		{
			this->m_Owner = owner;
		}

		~ComponentManager()
		{
			Clear();
		}

		void Clear() {
			delete[] m_NamesOfMembers;

			std::vector<ScriptComponent*> scripts = GetScripts();
			for (uint32_t i = 0; i < scripts.size(); i++)
			{
				RemoveComponent(scripts[i]);
			}

			for (uint32_t i = 0; i < m_Components.size(); i)
			{
				RemoveComponent(m_Components[i]);
			}
		}

		void operator=(ComponentManager& rhs)
		{
			Copy(rhs);
		}

		inline int GetAmountOfComponents() { return m_Components.size(); }

		const char** GetItems() 
		{
			if (m_NamesOfMembers != nullptr)
				delete[] m_NamesOfMembers;
			m_NamesOfMembers = new const char* [m_Components.size()];
			for (unsigned int i = 0; i < m_Components.size(); i++)
			{
				m_NamesOfMembers[i] = m_Components[i]->GetComponentType().c_str();
			}
			return m_NamesOfMembers;
		}

		std::vector<ScriptComponent*> GetScripts() {
			std::vector<ScriptComponent*> scripts;
			for (Component* com : m_Components)
			{
				if (com->m_Type == Utils::GetComponentTypeName<ScriptComponent>())
					scripts.push_back(static_cast<ScriptComponent*>(com));
			}
			return scripts;
		}

		template <class T>
		void RemoveComponent() 
		{
			T* com = nullptr;
			com = GetComponent<T>();
			if (com == nullptr) return;
			int _id = com->m_Id;
			if (com->m_Id < m_Components.size())
				m_Components.erase(m_Components.begin() + com->m_Id);
			unsigned int _size = m_Components.size();
			if (com->m_Id != _size) 
			{
				for (unsigned int i = com->m_Id; i < _size; i++)
				{
					m_Components[i]->m_Id = i;
				}
			}
			com->Delete();
			return;
		}

		void RemoveComponent(Component* com)
		{
			if (com == nullptr)
				return;
			int _id = com->m_Id;
			if (com->m_Id < m_Components.size())
				m_Components.erase(m_Components.begin() + com->m_Id);
			unsigned int _size = m_Components.size();
			if (com->m_Id != _size) 
			{
				for (unsigned int i = com->m_Id; i < _size; i++)
				{
					m_Components[i]->m_Id = i;
				}
			}
			com->Delete();
			return;
		}

		Component* GetComponent(const std::string& type)
		{
			for (auto com : m_Components)
			{
				if (com->m_Type == type)
				{
					return (com);
				}
			}
			return nullptr;
		}

		template<class T>
		T* GetComponent()
		{
			for (auto com : m_Components) 
			{
				if (com->m_Type == Utils::GetComponentTypeName<T>())
				{
					return static_cast<T*>(com);
				}
			}
//I wouldn't wanna see that 'cause it can be called every frame
//#ifdef _DEBUG
//			std::cout << BOLDYELLOW << "WARNING:" << NAMECOLOR << m_Owner->m_Name << RESET << ": has no <" NAMECOLOR << Utils::GetComponentTypeName<T>() << RESET << "> component" << std::endl;
//#endif
			return nullptr;
		}

		template<>
		Transform* GetComponent<Transform>();

		template<class T>
		T* AddComponent() 
		{
			if ((GetComponent<T>() != nullptr && GetComponent<T>()->m_Type == Utils::GetComponentTypeName<ScriptComponent>()) || GetComponent<T>() == nullptr)
			{
				Component* object = T::Create();
				object->m_OwnerOfCom = (this->m_Owner);
				object->SetType(Utils::GetComponentTypeName<T>());
				object->m_Id = m_Components.size();
				m_Components.push_back(object);
#ifdef _DEBUG
				std::cout << NAMECOLOR << Utils::GetComponentTypeName<T>() << " size:" << sizeof(T) << RESET << ": has been added to GameObject <" NAMECOLOR << m_Owner->m_Name << RESET << ">" << std::endl;
#endif
				return static_cast<T*>(object);
			}
			return GetComponent<T>();
		}
	};
}