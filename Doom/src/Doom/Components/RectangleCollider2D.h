#pragma once

#include "../ECS/Component.h"
#include "../Core/EventSystem.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Enums/ColorsRGBA.h"
#include "../OpenGl/Shader.h"
#include "Transform.h"
#include <mutex>

namespace Doom {

	class DOOM_API RectangleCollider2D : public Component {
	private:

		static std::map<char*, uint64_t> s_MemoryPool;
		static std::vector<char*> s_FreeMemory;

		static float m_Vertices[8];

		RectangleCollider2D* m_Col = nullptr;

		char* m_MemoryPoolPtr = nullptr;

		glm::vec2 m_Displacement;
		glm::vec2 m_Offset = { 0,0 };

		std::vector<glm::vec2> p;

		bool ShapeOverlap_SAT_STATIC(RectangleCollider2D &r1, RectangleCollider2D &r2);
		bool ShapeOverlap_SAT(RectangleCollider2D &r1, RectangleCollider2D &r2);

		void CalculateRealVerPos();
		void CalculateEdges();
		float* GetVertexPositions(); //8 floats

		friend class GameObject;
		friend class Transform;
		friend class Editor;
		friend class Renderer;
		friend class Batch;
		friend class Ray2D;
		friend class SceneSerializer;
		friend class ComponentManager;
	public:

		float* m_WorldSpaceVertices = new float[16];

		static std::vector <RectangleCollider2D*> s_Collision2d;

		RectangleCollider2D* m_CollidedObject = nullptr;

		static bool s_IsVisible;
		bool m_Enable = true;
		bool m_IsCollided = false;
		bool m_IsTrigger = false;

		RectangleCollider2D();
		~RectangleCollider2D();

		virtual void OnMove() override { CalculateRealVerPos(); }
		virtual void OnRotate() override { CalculateRealVerPos(); }
		virtual void OnScale() override { CalculateRealVerPos(); }
		virtual void OnTranslate() override { CalculateRealVerPos(); }

		static Component* Create();
		virtual void Delete() override;

		void SetOffset(float x, float y);
		void SetOffset(glm::vec2 offset);
		void IsCollidedSAT();
		void IsCollidedDIAGS();
	};

}