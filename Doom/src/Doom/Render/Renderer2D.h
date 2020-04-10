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
		};
	public:

		std::vector<void*> GetChilds() { return Childs; }
		void AddChild(void* child) { Childs.push_back(child); }
		void RemoveChild(void* child) {
			for (size_t i = 0; i < Childs.size(); i++)
			{
				if (Childs[i] == child) {
					Childs.erase(Childs.begin() + i);
					return;
				}
			}
		}
		void* GetOwner() { return Owner; }
		void SetOwner(void* owner) { this->Owner = owner; }

		bool Enable = true;
		virtual void Setlayer(int layer);
		inline virtual int GetId() { return 0; }
		enum RenderType { Render2D, Render3D };
		int& GetLayer() { return layer; }

		friend class Renderer;
		friend class Collision;
		friend class ComponentManager;
		friend class Transform;
		friend class Batch;
	protected:
		Position position;
		virtual ~Renderer2DLayer() {}
		Renderer2DLayer() { layer = objects2d.size(); }
		int id = 0;
		int layer = 0;
		static void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
		std::string* type = new std::string("");
		std::string* name = new std::string("");
		static std::vector <std::reference_wrapper<Renderer2DLayer>> collision2d;
		static std::vector <std::reference_wrapper<Renderer2DLayer>> objects2d;
		static int obj_id;
		static int col_id;
		RenderType rendertype = Render2D;
		void PushObj(Renderer2DLayer& obj) { objects2d.push_back(obj); }
		void PushCol(Renderer2DLayer& col) { collision2d.push_back(col); }
		virtual void OnRunning(OrthographicCamera& camera);
		inline virtual void SetId(int id) {}
		inline virtual float* GetVertexPositions() { return nullptr; }
		inline virtual float* GetColor() { return nullptr; }
		inline virtual float* GetScale() { return nullptr; }
		const char** Renderer2DLayer::GetItemsNames();
		inline virtual float GetAngle() { return 0; }
		inline virtual Position GetPositions() { return position; }
		inline virtual int GetRenderType() { return rendertype; }
		inline virtual int GetShaderType() { return 1; }
		inline virtual std::string* GetPathToTexture() { return nullptr; }
		inline virtual bool IsCollisionEnabled() { return false; }
		inline virtual Renderer2DLayer* GetCollisionReference() { return nullptr; }
		inline virtual ComponentManager* GetComponentManager() { return nullptr; };
	};

}

#endif

