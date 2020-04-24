#ifndef  COLLISION_H
#define COLLISION_H

#include "../ECS/Component.h"
#include "../Core/EventSystem.h"
#include "Transform.h"
#include <mutex>

namespace Doom {

	class DOOM_API Collision : public Renderer2DLayer, public Component {
	private:
		std::mutex mtx;
		const float positions[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f };
		const unsigned int indeces[6] = { 0,1,2,3,2,0 };

		Collision* col = nullptr;
		Transform* trans = nullptr;

		float scaleValues[3] = { 1,1,1 };

		struct Displacement {
			float x = 0;
			float y = 0;
		};

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

		GLuint vao;
		VertexBufferLayout* layout = new VertexBufferLayout;
		VertexBuffer* vb = new VertexBuffer(positions, 4 * 4 * sizeof(float));
		VertexArray* va = new VertexArray;
		IndexBuffer* ib = new IndexBuffer(indeces, 6);
		Shader* shader = nullptr;

		virtual void OnRunning(OrthographicCamera& camera) override;
		virtual bool IsCollisionEnabled() override { return Enable; }
		void UpdateCollision(double x, double y, glm::mat4 pos, glm::mat4 view, glm::mat4 scale);
		virtual Renderer2DLayer* GetCollisionReference() override { return this; }
		void RealVerPos();
		float* GetVertexPositions();
		virtual Position GetPositions() override {
			return position;
		};

		friend class GameObject;
		friend class Transform;
		friend class Editor;
		friend class Renderer;
		friend class Batch;
		friend class Ray;

		Displacement displacement;

		float offsetX = 0, offsetY = 0;
		float* ScaledVerPos = new float[16];
		std::vector<glm::vec2> p;
		std::string tag = "General";

		bool Collided_side_left = false;
		bool Collided_side_right = false;
		bool Collided_side_top = false;
		bool Collided_side_bottom = false;
	public:
		Collision* collidedObject = nullptr;
		std::string GetTag() { return tag; }
		virtual void SetId(int id)override { this->id = id; }
		virtual int GetId()override { return id; }

		static bool IsVisible;
		bool isCollided = false;
		bool IsTrigger = false;

		Collision(GameObject* owner = nullptr,double x = 0, double y = 0);

		void SetTag(std::string tag) { this->tag = tag; }
		void Scale(float x, float y);
		void Translate(float x, float y);
		bool Static = false;
		void SetOffset(float x, float y);
		bool IsCollidedSAT();
		void IsCollidedDIAGS();
	};

}
#endif