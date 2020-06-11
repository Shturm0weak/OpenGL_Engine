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

		ComponentManager* component_manager;

		float* GetScale();

		friend class Doom::Transform;
		friend class Doom::Collision;
		friend class Doom::ComponentManager;
		friend class Doom::Editor;
		friend class Doom::Animator;
		friend class Doom::Batch;
		friend class Doom::Renderer;
		friend class Doom::Ray;

	public:

		struct Position {
			float x = 0;
			float y = 0;
			float z = 0;
		};

		Position position;

		uint32_t id = 0;
		int layer = 0;

		float scaleValues[3] = { 1,1,1 };

		std::string type;
		std::string name;

		std::vector<void*> GetChilds() { return Childs; }

		float GetAngle();
		void* GetOwner() { return Owner; }
		void SetObjectType(std::string _type) { type = _type; }
		void AddChild(void* child) { Childs.push_back(child); }
		void SetOwner(void* owner) { this->Owner = owner; }
		void RemoveChild(void* child);
		void operator=(GameObject& go);
		void SetName(const char* _name);

		bool Enable = true;
		bool isParticle = false;
		bool Static = false;
		
		virtual ~GameObject();
		explicit GameObject(const std::string name = "Unnamed", float x = 0, float y = 0);
		
		Position GetPositions();
		ComponentManager* GetComponentManager(){ return component_manager; };

		inline double GetWidth() { return scaleValues[0] * GetComponentManager()->GetComponent<SpriteRenderer>()->mesh2D[4] * 2; }
		inline double GetHeight() { return scaleValues[1] * GetComponentManager()->GetComponent<SpriteRenderer>()->mesh2D[9] * 2; }
		inline void SetId(int id) { this->id = id; }
		inline int GetId() { return id; }
		inline int& GetLayer() { return layer; }
		
	};

}

#endif 

