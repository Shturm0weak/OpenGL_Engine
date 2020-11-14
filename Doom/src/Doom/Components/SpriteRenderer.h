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

		static unsigned int indeces2D[6];
		float mesh2D[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
		};

		Transform* tr = nullptr;


		inline std::string GetPathToTexture() const { return this->pathToTexture; };
		std::string pathToTexture = ("None");

		friend class Transform;
		friend class GameObject;
		friend class Batch;
		friend class Renderer;
		friend class SceneSerializer;
		friend class ComponentManager;
	
	public:
		Texture* texture = Texture::WhiteTexture;
		SpriteRenderer();
		~SpriteRenderer();

		virtual void Update(glm::vec3 pos) override;
		virtual void Render() override;

		float WorldVertexPositions[8] = {
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			 0.5f,  0.5f,
			-0.5f,  0.5f
		};

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