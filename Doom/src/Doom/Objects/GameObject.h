#pragma once

#include "../ECS/ComponentManager.h"
#include "../Core/Listener.h"
#include "../Core/Event.h"
#include "../Core/Window.h"
#include "../Render/TextureAtlas.h"
#include "../Components/SpriteRenderer.h"

namespace Doom {

	class DOOM_API GameObject
	{
	private:

		static std::map<char*, uint64_t> s_MemoryPool;
		static std::vector<char*> s_FreeMemory;
		std::vector<void*> m_Childs;
		void* m_Owner = nullptr;
		char* m_MemoryPoolPtr = nullptr;

		void Copy(GameObject& rhs);

		explicit GameObject(const std::string name = "Unnamed", float x = 0, float y = 0, float z = 0);

		void Delete();

		friend class Doom::Transform;
		friend class Doom::RectangleCollider2D;
		friend class Doom::ComponentManager;
		friend class Doom::Editor;
		friend class Doom::Animator;
		friend class Doom::Batch;
		friend class Doom::Renderer;
		friend class Doom::Ray2D;
		friend class Doom::World;
		friend class Doom::SkyBox;
		friend class ParticleSystem;
		friend class GridLayOut;

		template<typename T> friend DOOM_API char* Utils::PreAllocateMemory(std::map<char*, uint64_t>& memoryPool, std::vector<char*>& freeMemory);

	public:
		
		int m_Id = 0;
		int m_Layer = 0;
		bool m_IsStatic = false;
		bool m_Enable = true;
		bool m_IsSerializable = true;
		bool m_IsSerializalbeChilds = true;
		bool m_IsParticle = false;

		ComponentManager m_ComponentManager = ComponentManager(this);
		Transform m_Transform = Transform(this);
		Listener* m_Listener = nullptr;

		std::string m_Name = "Unnamed";
		std::string m_Tag = "General";

		std::vector<void*> GetChilds() const { return m_Childs; }
		glm::vec3 GetScale();
		glm::vec3 GetPosition();
		inline int& GetLayer() { return m_Layer; }
		void* GetOwner() const { return m_Owner; }
		void AddChild(void* child) { m_Childs.push_back(child); static_cast<GameObject*>(child)->m_Owner = this; }
		void SetOwner(void* owner) { this->m_Owner = owner; static_cast<GameObject*>(owner)->AddChild(this); }
		void RemoveChild(void* child);

		template <typename T>
		T* GetComponent() 
		{
			return m_ComponentManager.GetComponent<T>();
		}

		template<typename T>
		T* AddComponent() 
		{
			return m_ComponentManager.AddComponent<T>();
		}

		template<typename T>
		void RemoveComponent()
		{
			m_ComponentManager.RemoveComponent<T>();
		}

		virtual ~GameObject();

		GameObject(GameObject& rhs);
		void operator=(GameObject& rhs);
		static GameObject* Create(const std::string name = "Unnamed", float x = 0, float y = 0, float z = 0);
	};

}