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

		std::vector<void*> Childs;
		void* Owner = nullptr;
		const char** items = nullptr;

		ComponentManager* component_manager = nullptr;

		friend class Doom::Transform;
		friend class Doom::RectangleCollider2D;
		friend class Doom::ComponentManager;
		friend class Doom::Editor;
		friend class Doom::Animator;
		friend class Doom::Batch;
		friend class Doom::Renderer;
		friend class Doom::Ray2D;

	public:

		Transform* transform = nullptr;

		int id = 0;
		int layer = 0;
		bool Enable = true;
		bool isSerializable = true;
		bool isParticle = false;

		std::string name;

		std::vector<void*> GetChilds() const { return Childs; }
		void* GetOwner() const { return Owner; }
		void AddChild(void* child) { Childs.push_back(child); }
		void SetOwner(void* owner) { this->Owner = owner; }
		void RemoveChild(void* child);

		template <typename T>
		T* GetComponent() {
			return component_manager->GetComponent<T>();
		}

		virtual ~GameObject();
		explicit GameObject(const std::string name = "Unnamed", float x = 0, float y = 0,float z = 0);
		
		glm::vec3 GetScale();
		glm::vec3 GetPosition();
		ComponentManager* GetComponentManager() const { return component_manager; };
		inline int& GetLayer() { return layer; }
		
	};

}

#endif 

