#pragma once
#ifndef  SPRITERENDERER_H
#define SPRITERENDERER_H

#include "IRenderer.h"
#include "Transform.h"
#include "../Core/Core.h"
#include "../Render/Texture.h"
#include "../Render/TextureAtlas.h"
#include "../Core/ColorsRGBA.h"
#include "../Render/Shader.h"

namespace Doom {

	class DOOM_API SpriteRenderer : public Irenderer {
	private:

		std::mutex updateLock;

		unsigned int indeces2D[6] = { 0,1,2,3,2,0 };
		float mesh2D[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
		};

		Transform* tr = nullptr;


		inline std::string GetPathToTexture() const { return this->pathToTexture; };
		std::string pathToTexture = ("None");

		void InitShader();

		friend class Transform;
		friend class GameObject;
		friend class Batch;
		friend class Renderer;
	
	public:
		Texture* texture = nullptr;
		SpriteRenderer(GameObject* owner);
		~SpriteRenderer();

		virtual void Update(glm::vec3 pos) override;
		virtual void Render() override;

		float WorldVertexPositions[8] = {
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			 0.5f,  0.5f,
			-0.5f,  0.5f
		};

		bool AlwaysDraw = false;

		inline int GetTexture() { return texture->m_RendererID; }
		inline Texture* GetTexturePointer() { return texture; }
		double GetWidth() const;
		double GetHeight() const;

		float* GetUVs() const;
		

		//Only in int size from 0 to 1 !!!
		void ReversedUvs();
		void OriginalUvs();
		void ReverseUVs();
		void SetUVs(float* uvs);
		void SetTexture(Texture* texture);
		void Setlayer(int layer);

	};
}
#endif