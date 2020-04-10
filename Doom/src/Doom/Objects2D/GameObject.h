#ifndef BASIC2D_H
#define BASIC2D_H

#include "../ECS/ComponentManager.h"
#include "../Core/Listener.h"
#include "../Core/Event.h"
#include "../Core/Window.h"

namespace Doom {

	class DOOM_API GameObject : public Renderer2DLayer, public Listener
	{
	public:
		enum ShaderType { SHADER_TEXTURE, SHADER_COLOR, SHADER_BLOOM };
	private:
		ShaderType shadertype = SHADER_COLOR;
		ComponentManager* component_manager;
		bool draw_once = false;

		glm::mat4 ViewProjecTionRelatedToScreen = glm::ortho(-5.f, 5.f, -3.75f, 3.75f, 1.f, -1.f);

		void InitShader();
		virtual void OnRunning(OrthographicCamera& camera) override;
		virtual Renderer2DLayer* GetCollisionReference() override { return component_manager->GetComponent<Collision>(); }
		virtual std::string* GetPathToTexture()override;
		virtual int GetRenderType() override;
		virtual float* GetScale() override;
		virtual void SetId(int id) override { this->id = id; }

		std::string* pathToTexture = new std::string("none");

		unsigned int indeces2D[6] = { 0,1,2,3,2,0 };
		float mesh2D[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
		};

		unsigned int indeces3D[36] = {// front
			0, 1, 2,
			2, 3, 0,
			// right
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// left
			4, 0, 3,
			3, 7, 4,
			// bottom
			4, 5, 1,
			1, 0, 4,
			// top
			3, 2, 6,
			6, 7, 3 };
		float mesh3D[24] = {
		-0.5, -0.5,  0.5,
		 0.5, -0.5,  0.5,
		 0.5,  0.5,  0.5,
		-0.5,  0.5,  0.5,
		// back
		-0.5, -0.5, -0.5,
		 0.5, -0.5, -0.5,
		 0.5,  0.5, -0.5,
		-0.5,  0.5, -0.5
		};

	protected:
		glm::mat4 scale = glm::mat4(1.f);
		glm::mat4 pos = glm::mat4(1.f);
		glm::mat4 viewXscale = glm::mat4(1.f);
		glm::mat4 MVP = glm::mat4(1.f);
		glm::mat4 view = glm::mat4(1.f);
		glm::vec2* submitedVectors = new glm::vec2[4];

	private:
		glm::vec4 color = COLORS::White;
		GLuint vao;
		VertexBufferLayout* layout = new VertexBufferLayout;
		VertexBuffer* vb = new VertexBuffer(mesh2D, 4 * 4 * sizeof(float));
		VertexArray* va = new VertexArray;
		IndexBuffer* ib = new IndexBuffer(indeces2D, 6);
		Texture* texture = nullptr;
		Shader* shader = nullptr;

		friend class Doom::Transform;
		friend class Doom::Collision;
		friend class Doom::ComponentManager;
		friend class Doom::Editor;
		friend class Doom::Animator;
		friend class Doom::Batch;
		friend class Doom::Renderer2DLayer;
		friend class Doom::Renderer;

	public:
		float scaleValues[3] = { 1,1,1 };
		bool isParticle = false;
		bool Static = false;
		
		~GameObject() { EventSystem::Instance()->UnregisterAll(this); }

		int GetTexture() { return texture->m_RendererID; }
		void operator=(GameObject go);
		using Renderer2DLayer::name;
		explicit GameObject(const std::string name = "Unnamed", float x = 0, float y = 0);
		void SetName(const char* _name);
		void SetTexture(const std::string& path);
		void SetTexture(Texture* texture);
		void SetColor(vec4 color);
		void SetShader(int _enum);
		void SetRenderType(RenderType type);
		inline double GetWidth() { return scaleValues[0] * mesh2D[4] * 2; }
		inline double GetHeight() { return scaleValues[1] * mesh2D[9] * 2;; }
		virtual Position GetPositions() override;
		virtual int GetShaderType() override;
		virtual int GetId()override { return id; }
		virtual float GetAngle() override;
		virtual float* GetColor() override;
		virtual ComponentManager* GetComponentManager() override { return component_manager; };
	};

}

#endif 

