#pragma once
#include "../pch.h"
#include "Renderer.h"
#include "Text/Character.h"
#include "Objects/Line.h"
#include <array>
#include <mutex>

namespace Doom {

#define RENDERER_MAX_SPRITES   1000
#define RENDERER_INDICES_SIZE  RENDERER_MAX_SPRITES * 6

	class DOOM_API Batch {
	private:

		struct TextVertex {
			glm::vec2 m_Vertex;
			glm::vec2 m_UV;
			int m_Static;
			glm::vec4 m_Color;
			float m_IsGui;
			float m_TexIndex;
		};

		struct GOVertex {
			glm::vec2 m_Vertex;
			glm::vec2 m_UV;
			glm::vec4 m_Color;
			float m_TexIndex;
			glm::vec4 m_RotationMat0;
			glm::vec4 m_RotationMat1;
			glm::vec4 m_RotationMat2;
			glm::vec4 m_RotationMat3;
			glm::vec3 m_Pos;
		};

		struct VertexLine {
			glm::vec3 m_Vertex;
			glm::vec4 m_Color;
			glm::vec4 m_RotationMat0;
			glm::vec4 m_RotationMat1;
			glm::vec4 m_RotationMat2;
			glm::vec4 m_RotationMat3;
			glm::vec4 m_MVPMat0;
			glm::vec4 m_MVPMat1;
			glm::vec4 m_MVPMat2;
			glm::vec4 m_MVPMat3;
		};

		struct VertAttribWrapper {
			std::array<uint32_t, 32> m_TextureSlots;
			unsigned int m_TextureSlotsIndex = 2;
			float m_TextureIndex = 0;

			VertexLine* m_BufferL = nullptr;
			VertexLine* m_BufferPtrL = nullptr;
			TextVertex* m_BufferT = nullptr;
			TextVertex* m_BufferPtrT = nullptr;
			GOVertex* m_BufferG = nullptr;
			GOVertex* m_BufferPtrG = nullptr;
			GLuint m_Vao;
			GLuint m_Vbo;
			IndexBuffer* m_Ibo;

			~VertAttribWrapper()
			{
				delete m_Ibo;
				glDeleteBuffers(1, &m_Vbo);
			}
		};

		VertAttribWrapper m_TextB;
		VertAttribWrapper m_GoB;
		VertAttribWrapper m_LinesB;

		Shader* m_Shader = nullptr;
		const unsigned int maxTextureSlots = 32;
		bool m_IsBegan = false;
		void InitText();
		void InitGameObjects();
		void InitLines();

		Batch();
		Batch(const Batch&) = delete;
		Batch& operator=(const Batch&) { return *this; }
		~Batch();
	public:

		Shader* m_TextShader = Shader::Get("Font");
		Shader* m_BasicShader = Shader::Get("Default2D");
		Shader* m_CollisionShader = Shader::Get("Collision2D");
		Shader* m_LineShader = Shader::Get("Line");
		GLsizei m_TIndexCount = 0;
		GLsizei m_GIndexCount = 0;
		GLsizei m_LIndexCount = 0;

		static Batch& GetInstance();

		void Submit(Character* character);
		void Submit(glm::mat4 pos,glm::mat4 view, glm::vec4 color,glm::vec2 scale, Texture* texture = nullptr);
		void Submit(float* mesh2D, glm::vec4 color,Texture* texture = nullptr,glm::vec2 size = glm::vec2(0), glm::vec2 pos = glm::vec2(0),float raduis = 0);
		void Submit(SpriteRenderer* gameobject);
		void Submit(Line& line);
		void Submit(glm::vec4 color, float* mesh);
		void Submit(RectangleCollider2D& collision);
		void FlushGameObjects(Shader* shader);
		void FlushCollision(Shader* shader);
		void FlushText(Shader* shader);
		void FlushLines(Shader* shader);
		//bool isValidBuffer() { return (m_GoB.m_Buffer != nullptr && m_IsBegan && m_GoB.m_Buffer != (void*)0x00000000); }

		void BeginText();
		void EndText();
		void BeginGameObjects();
		void EndGameObjects();
		void BeginLines();
		void EndLines();

		friend class Gizmos;
	};

}