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

namespace Doom {

	class DOOM_API RectangleCollider2D : public Component {
	private:
		std::mutex mtx;
		const float positions[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f };
		const unsigned int indeces[6] = { 0,1,2,3,2,0 };

		int id = 0;
		//void* Owner = nullptr;
		RectangleCollider2D* col = nullptr;
		Transform* trans = nullptr;
		
		glm::vec3 scaleValues = { 1,1,1 };
		glm::vec2 position;

		float* arrver1 = nullptr;
		double* arrpos1 = nullptr;
		double left, right, top, bottom;
		double diffbt, difftb, diffrl, difflr;


		glm::vec4 Color = COLORS::White;
		glm::mat4 scale = glm::mat4(1.f);
		glm::mat4 pos = glm::mat4(1.f);
		glm::mat4 MVP = glm::mat4(1.f);
		glm::mat4 view = glm::mat4(1.f);
		glm::mat4 scaleXview = glm::mat4(1.f);
		glm::vec2 NormalVector(glm::vec2 vec2);

		Shader* shader = nullptr;

		bool ShapeOverlap_SAT_STATIC(RectangleCollider2D &r1, RectangleCollider2D &r2);
		bool ShapeOverlap_SAT(RectangleCollider2D &r1, RectangleCollider2D &r2);
		void UpdateCollision(double x, double y, glm::mat4 pos, glm::mat4 view, glm::mat4 scale);
		void RealVerPos();
		float* GetVertexPositions();
		

		friend class GameObject;
		friend class Transform;
		friend class Editor;
		friend class Renderer;
		friend class Batch;
		friend class Ray2D;

		glm::vec2 displacement;
		glm::vec2 offset = {0,0};

		std::vector<glm::vec2> p;
		std::string tag = "General";

		bool Collided_side_left = false;
		bool Collided_side_right = false;
		bool Collided_side_top = false;
		bool Collided_side_bottom = false;
	public:
		void SetOwner(GameObject* _owner) { m_Owner = _owner; }

		inline glm::vec2 GetPositions() const {
			return position;
		};

		float* ScaledVerPos = new float[16];

		RectangleCollider2D* collidedObject = nullptr;
		std::string GetTag() const { return tag; }
		void SetId(int id) { this->id = id; }
		int GetId() const { return id; }

		static bool IsVisible;
		bool Enable = true;
		bool isCollided = false;
		bool IsTrigger = false;

		RectangleCollider2D(GameObject* owner = nullptr,double x = 0, double y = 0);
		~RectangleCollider2D();

		void SetTag(std::string tag) { this->tag = tag; }
		void Scale(float x, float y);
		void Translate(float x, float y);
		bool Static = false;
		void SetOffset(float x, float y);
		void IsCollidedSAT();
		void IsCollidedDIAGS();
	};

}
#endif