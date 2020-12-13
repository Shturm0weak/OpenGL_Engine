#ifndef BASIC2D_H
#define BASIC2D_H

#include "../ECS/ComponentManager.h"
#include "../Core/Listener.h"
#include "../Core/Event.h"
#include "../Core/Window.h"
#include "../Render/TextureAtlas.h"
#include "../Components/SpriteRenderer.h"

namespace Doom {

	class DOOM_API GameObject :public Listener
	{
	private:

		std::vector<void*> m_Childs;
		ComponentManager* m_ComponentManager = nullptr;
		void* m_Owner = nullptr;

		void Copy(const GameObject& rhs);

		friend class Doom::Transform;
		friend class Doom::RectangleCollider2D;
		friend class Doom::ComponentManager;
		friend class Doom::Editor;
		friend class Doom::Animator;
		friend class Doom::Batch;
		friend class Doom::Renderer;
		friend class Doom::Ray2D;

	public:

		Transform* m_Transform = nullptr;

		std::string m_Name;
		std::string m_Tag;
		int m_Id = 0;
		int m_Layer = 0;
		bool m_Enable = true;
		bool m_IsSerializable = true;
		bool m_IsParticle = false;

		std::vector<void*> GetChilds() const { return m_Childs; }
		glm::vec3 GetScale();
		glm::vec3 GetPosition();
		ComponentManager* GetComponentManager() const { return m_ComponentManager; };
		inline int& GetLayer() { return m_Layer; }
		void* GetOwner() const { return m_Owner; }
		void AddChild(void* child) { m_Childs.push_back(child); static_cast<GameObject*>(child)->m_Owner = this; }
		void SetOwner(void* owner) { this->m_Owner = owner; static_cast<GameObject*>(owner)->AddChild(this); }
		void RemoveChild(void* child);

		template <typename T>
		T* GetComponent() {
			return m_ComponentManager->GetComponent<T>();
		}

		virtual ~GameObject();
		explicit GameObject(const std::string name = "Unnamed", float x = 0, float y = 0,float z = 0);
		GameObject(const GameObject& rhs);

		void operator=(const GameObject& rhs);
	};

}

#endif 

