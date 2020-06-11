#ifndef RENDERER2D_H
#define RENDERER2D_H

#include <mutex>
#include "OrthographicCamera.h"

namespace Doom {

	class DOOM_API Renderer2DLayer {
		std::vector<void*> Childs;
		void* Owner = nullptr;
		const char** items = nullptr;

	public:
		struct Position {
			float x = 0;
			float y = 0;
			float z = 0;
		};

	public:

		bool Enable = true;
		
		friend class Renderer;
		friend class Collision;
		friend class ComponentManager;
		friend class Transform;
		friend class Batch;
		friend class Ray;

		static std::vector <Renderer2DLayer*> collision2d;
		static std::vector <Renderer2DLayer*> objects2d;

		std::vector<void*> GetChilds() { return Childs; }
		void SetObjectType(std::string _type) { type = _type; }
		void AddChild(void* child) { Childs.push_back(child); }
		void* GetOwner() { return Owner; }
		void SetOwner(void* owner) { this->Owner = owner; }
		void RemoveChild(void* child);
		virtual void Setlayer(int layer);
		inline virtual int GetId() { return 0; }
		int& GetLayer() { return layer; }

	protected:
		Position position;
		
		int id = 0;
		int layer = 0;
		std::string type;
		std::string name;
		
		static int obj_id;
		static int col_id;

		Renderer2DLayer() { layer = objects2d.size(); }
		virtual ~Renderer2DLayer() {}
		static void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
		virtual void OnRunning(OrthographicCamera& camera);
		inline virtual void SetId(int id) {}
		inline virtual float* GetVertexPositions() { return nullptr; }
		inline virtual float* GetScale() { return nullptr; }
		const char** GetItemsNames();
		inline virtual float GetAngle() { return 0; }
		inline virtual Position GetPositions() { return position; }
		inline virtual std::string GetPathToTexture() { return nullptr; }
		inline virtual bool IsCollisionEnabled() { return false; }
		inline virtual Renderer2DLayer* GetCollisionReference() { return nullptr; }
		inline virtual ComponentManager* GetComponentManager() { return nullptr; };
	};

}

#endif

