#ifndef BASIC2D_H
#define BASIC2D_H

#include "../ECS/ComponentManager.h"
#include "../Core/Listener.h"
#include "../Core/Event.h"
#include "../Core/Window.h"
#include "../Render/TextureAtlas.h"
#include "../Components/SpriteRenderer.h"

namespace Doom {

	class DOOM_API GameObject : public Renderer2DLayer, public Listener
	{
	public:

		bool AlwaysDraw = false;
	private:
		ComponentManager* component_manager;

		virtual Renderer2DLayer* GetCollisionReference() override { return component_manager->GetComponent<Collision>(); }
		
		virtual float* GetScale() override;
		virtual void SetId(int id) override { this->id = id; }

	private:

		friend class Doom::Transform;
		friend class Doom::Collision;
		friend class Doom::ComponentManager;
		friend class Doom::Editor;
		friend class Doom::Animator;
		friend class Doom::Batch;
		friend class Doom::Renderer2DLayer;
		friend class Doom::Renderer;
		friend class Doom::Ray;

	public:

		float scaleValues[3] = { 1,1,1 };
		bool isParticle = false;
		bool Static = false;
		
		~GameObject();

		void operator=(GameObject& go);
		using Renderer2DLayer::name;
		explicit GameObject(const std::string name = "Unnamed", float x = 0, float y = 0);
		void SetName(const char* _name);
		virtual Position GetPositions() override;
		virtual int GetId()override { return id; }
		virtual float GetAngle() override;
		virtual ComponentManager* GetComponentManager() override { return component_manager; };

		inline double GetWidth() { return scaleValues[0] * GetComponentManager()->GetComponent<SpriteRenderer>()->mesh2D[4] * 2; }
		inline double GetHeight() { return scaleValues[1] * GetComponentManager()->GetComponent<SpriteRenderer>()->mesh2D[9] * 2; }
	};

}

#endif 

