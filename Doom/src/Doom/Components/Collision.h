#ifndef  COLLISION_H
#define COLLISION_H

#include "../ECS/Component.h"
#include "../Core/EventSystem.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Core/ColorsRGBA.h"
#include "../Render/Shader.h"
#include "Transform.h"
#include <mutex>

namespace Doom {

	class DOOM_API Collision : public Component {
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
		Collision* col = nullptr;
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

		bool ShapeOverlap_SAT_STATIC(Collision &r1, Collision &r2);
		bool ShapeOverlap_SAT(Collision &r1, Collision &r2);
		void UpdateCollision(double x, double y, glm::mat4 pos, glm::mat4 view, glm::mat4 scale);
		void RealVerPos();
		float* GetVertexPositions();
		

		friend class GameObject;
		friend class Transform;
		friend class Editor;
		friend class Renderer;
		friend class Batch;
		friend class Ray;

		glm::vec2 displacement;
		glm::vec2 offset = {0,0};

		std::vector<glm::vec2> p;
		std::string tag = "General";

		bool Collided_side_left = false;
		bool Collided_side_right = false;
		bool Collided_side_top = false;
		bool Collided_side_bottom = false;
	public:
		void SetOwner(GameObject* _owner) { owner = _owner; }

		glm::vec2 GetPositions(){
			return position;
		};

		float* ScaledVerPos = new float[16];

		Collision* collidedObject = nullptr;
		std::string GetTag() { return tag; }
		void SetId(int id) { this->id = id; }
		int GetId() { return id; }

		static bool IsVisible;
		bool Enable = true;
		bool isCollided = false;
		bool IsTrigger = false;

		Collision(GameObject* owner = nullptr,double x = 0, double y = 0);
		~Collision() {
			//std::cout << "Collision destroyed\n";
		}

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