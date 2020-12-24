#ifndef  COLLISION_H
#define COLLISION_H

#include "../ECS/Component.h"
#include "../Core/EventSystem.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Enums/ColorsRGBA.h"
#include "../OpenGl/Shader.h"
#include "Transform.h"
#include <mutex>

//!!! Deprecated !!!\\
//!!! Need to be added before <SpriteRenderer> otherwise it will be considered in the (0,0) coordinates !!!\\

namespace Doom {

	class DOOM_API RectangleCollider2D : public Component {
	private:

		const float m_Vertices[8] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f };

		static std::vector<RectangleCollider2D*> s_CollidersToInit;
		static Shader* s_Shader;

		RectangleCollider2D* m_Col = nullptr;
		Transform* m_Transform = nullptr;


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

		glm::vec2 m_Displacement;
		glm::vec2 m_Offset = {0,0};

		std::vector<glm::vec2> p;

	public:

		float* m_TransformedVerPos = new float[16];

		static std::vector <RectangleCollider2D*> s_Collision2d;

		RectangleCollider2D* m_CollidedObject = nullptr;

		static bool s_IsVisible;
		bool m_Enable = true;
		bool m_IsCollided = false;
		bool m_IsTrigger = false;

		RectangleCollider2D(GameObject* owner = nullptr,double x = 0, double y = 0);
		~RectangleCollider2D();

		void SetOffset(float x, float y);
		void IsCollidedSAT();
		void IsCollidedDIAGS();

		static void CollidersToInit();
	};

}
#endif