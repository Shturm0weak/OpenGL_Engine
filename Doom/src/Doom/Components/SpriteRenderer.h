#pragma once
#ifndef  SPRITERENDERER_H
#define SPRITERENDERER_H

#include "IRenderer.h"
#include "Transform.h"
#include "../Core/Core.h"
#include "../OpenGl/Texture.h"
#include "../Render/TextureAtlas.h"
#include "../Enums/ColorsRGBA.h"
#include "../OpenGl/Shader.h"

namespace Doom {

	class DOOM_API SpriteRenderer : public Irenderer {
	private:

		static unsigned int s_Indices2D[6];
		float m_Mesh2D[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
		};

		Transform* m_Tr = nullptr;

		void Copy(const SpriteRenderer& rhs);
		inline std::string GetPathToTexture() const { return this->m_PathToTexture; };
		std::string m_PathToTexture = ("None");

		friend class Transform;
		friend class GameObject;
		friend class Batch;
		friend class Renderer;
		friend class SceneSerializer;
		friend class ComponentManager;
	
	public:

		float m_WorldVertexPositions[8] = {
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			 0.5f,  0.5f,
			-0.5f,  0.5f
		};

		Texture* m_Texture = Texture::s_WhiteTexture;

		SpriteRenderer(const SpriteRenderer& rhs);
		SpriteRenderer();
		~SpriteRenderer();

		void operator=(const SpriteRenderer& rhs);

		virtual void Update(glm::vec3 pos) override;
		virtual void Render() override;

		inline int GetTexture() { return m_Texture->m_RendererID; }
		inline Texture* GetTexturePointer() { return m_Texture; }
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