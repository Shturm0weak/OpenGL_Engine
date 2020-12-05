#pragma once
#include "../pch.h"
#include "Renderer.h"
#include "Text/Character.h"
#include "Objects/Line.h"
#include <array>
#include <mutex>

namespace Doom {

#define RENDERER_MAX_SPRITES   25000
#define RENDERER_VERTEX_SIZE   sizeof(Vertex)
#define RENDERER_SPRITE_SIZE   RENDERER_VERTEX_SIZE * 4
#define RENDERER_BUFFER_SIZE   RENDERER_SPRITE_SIZE * RENDERER_MAX_SPRITES
#define RENDERER_INDICES_SIZE  RENDERER_MAX_SPRITES * 6

	class DOOM_API Batch {
	private:

		struct Vertex {
			glm::vec2 vertex;
			glm::vec2 textcoords;
			int m_static;
			glm::vec4 m_color;
			float isGui;
			float texIndex;
			glm::vec4 rotationMat0;
			glm::vec4 rotationMat1;
			glm::vec4 rotationMat2;
			glm::vec4 rotationMat3;
			glm::vec4 posMat0;
			glm::vec4 posMat1;
			glm::vec4 posMat2;
			glm::vec4 posMat3;
			glm::vec2 pos;
			glm::vec2 size;
			glm::vec2 viewportSize;
			float raduis;
			glm::vec2 panelSize;
			glm::vec2 panelPos;
			int relatedToPanel;
		};

		struct VertexLine {
			glm::vec3 m_vertex;
			glm::vec4 m_color;
			glm::vec4 rotationMat0;
			glm::vec4 rotationMat1;
			glm::vec4 rotationMat2;
			glm::vec4 rotationMat3;
			glm::vec4 MVPMat0;
			glm::vec4 MVPMat1;
			glm::vec4 MVPMat2;
			glm::vec4 MVPMat3;
		};

		struct VertAttribWrapper {
			std::array<uint32_t, 32> m_TextureSlots;
			unsigned int m_TextureSlotsIndex = 2;
			float m_TextureIndex = 0;

			VertexLine* m_BufferL = nullptr;
			VertexLine* m_BufferPtrL = nullptr;
			Vertex* m_Buffer = nullptr;
			Vertex* m_BufferPtr = nullptr;
			GLuint m_Vao;
			GLuint m_Vbo;
			IndexBuffer* m_Ibo;

			~VertAttribWrapper() {
				delete m_Ibo;
				glDeleteBuffers(1, &m_Vbo);
			}
		};

		VertAttribWrapper m_TextB;
		VertAttribWrapper m_GoB;
		VertAttribWrapper m_LinesB;

		Shader* m_Shader = nullptr;
		static Batch* s_Instance;
		const unsigned int maxTextureSlots = 32;
		bool m_IsBegan = false;
		void initText();
		void initGameObjects();
		void initLines();
	public:

		Shader* m_TextShader = Shader::Get("Font");
		Shader* m_BasicShader = Shader::Get("Default2D");
		Shader* m_CollisionShader = Shader::Get("Collision2D");
		Shader* m_LineShader = Shader::Get("Line");
		GLsizei m_TIndexCount = 0;
		GLsizei m_GIndexCount = 0;
		GLsizei m_LIndexCount = 0;

		Batch();
		~Batch();

		static void Init();
		inline static Batch* GetInstance() { return s_Instance; }

		void Submit(Character* character);
		void Submit(glm::mat4 pos,glm::mat4 view, glm::vec4 color,glm::vec2 scale, Texture* texture = nullptr);
		void Submit(float* mesh2D, glm::vec4 color,Texture* texture = nullptr,glm::vec2 size = glm::vec2(0), glm::vec2 pos = glm::vec2(0),float raduis = 0);
		void Submit(SpriteRenderer& gameobject);
		void Submit(Line& line);
		void Submit(glm::vec4 color, float* mesh);
		void Submit(RectangleCollider2D& collision);
		void flushGameObjects(Shader* shader);
		void flushCollision(Shader* shader);
		void flushText(Shader* shader);
		void flushLines(Shader* shader);
		bool isValidBuffer() { return (m_GoB.m_Buffer != nullptr && m_IsBegan && m_GoB.m_Buffer != (void*)0x00000000); }

		void BeginText();
		void EndText();
		void BeginGameObjects();
		void EndGameObjects();
		void BeginLines();
		void EndLines();

		friend class Gizmos;
	};

}