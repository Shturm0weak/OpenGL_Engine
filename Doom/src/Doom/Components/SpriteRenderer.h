#pragma once
#ifndef  SPRITERENDERER_H
#define SPRITERENDERER_H

#include "Transform.h"
#include "../Core/Core.h"
#include "../Render/Texture.h"
#include "../Render/TextureAtlas.h"
#include "../Core/ColorsRGBA.h"
#include "../Render/Shader.h"

namespace Doom {

	class DOOM_API SpriteRenderer : public Component {
	private:

		unsigned int indeces2D[6] = { 0,1,2,3,2,0 };
		float mesh2D[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
		};

		glm::vec4 color = COLORS::White;

		Transform* tr = nullptr;
		Texture* texture = nullptr;
		Shader* shader = nullptr;

		std::string GetPathToTexture();
		std::string pathToTexture = ("None");

		void InitShader();

		friend class Transform;
		friend class GameObject;
		friend class Batch;
		friend class Renderer;

	protected:
		glm::mat4 scale = glm::mat4(1.f);
		glm::mat4 pos = glm::mat4(1.f);
		glm::mat4 viewXscale = glm::mat4(1.f);
		glm::mat4 MVP = glm::mat4(1.f);
		glm::mat4 view = glm::mat4(1.f);
	
	public:
		TextureAtlas* textureAtlas = nullptr;

		SpriteRenderer(GameObject* owner);
		~SpriteRenderer();

		float WorldVertexPositions[8] = {
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			 0.5f,  0.5f,
			-0.5f,  0.5f
		};

		bool AlwaysDraw = false;

		inline int GetTexture() { return texture->m_RendererID; }
		inline Texture* GetTexturePointer() { return texture; }

		float* GetUVs();
		float* GetColor();

		//Only in int size from 0 to 1 !!!
		void ReversedUvs();
		void OriginalUvs();
		void ReverseUVs();
		void SetUVs(float* uvs);
		void SetTexture(const std::string& path);
		void SetTexture(Texture* texture);
		void SetColor(vec4 color);
		void Setlayer(int layer);

	};
}
#endif