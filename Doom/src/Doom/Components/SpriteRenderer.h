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
#include "../Render/VertexArray.h"
#include "../Render/VertexBuffer.h"
#include "../Render/VertexBufferLayout.h"
#include "../Render/IndexBuffer.h"

namespace Doom {

	class DOOM_API SpriteRenderer : public Irenderer {
	private:

		unsigned int indeces2D[6] = { 0,1,2,3,2,0 };
		float mesh2D[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
		};

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
	
	public:
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
		double GetWidth();
		double GetHeight();

		float* GetUVs();
		

		//Only in int size from 0 to 1 !!!
		void ReversedUvs();
		void OriginalUvs();
		void ReverseUVs();
		void SetUVs(float* uvs);
		void SetTexture(const std::string& path);
		void SetTexture(Texture* texture);
		void Setlayer(int layer);

	};

	class DOOM_API Renderer3D : public Irenderer {
	public:

		Transform* tr = nullptr;

		Shader* shader = nullptr;
		unsigned int indeces3D[36] = {
			// front
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
			6, 7, 3
		};

		float mesh3D[56] = {
			-1.0, -1.0,  1.0, 1, 0, 0,			 1,
			 1.0, -1.0,  1.0,1, 1, 0,			 1,
			 1.0,  1.0,  1.0,0, 0, 1,			 1,
			-1.0,  1.0,  1.0,0, 1, 0,			 1,
			// back			 //					 
			-1.0, -1.0, -1.0,0.5, 0.3, 0.1,		 1,
			 1.0, -1.0, -1.0,1, 1, 1,			 1,
			 1.0,  1.0, -1.0,1, 0.31, 0,		 1,
			-1.0,  1.0, -1.0,0.86, 0.86, 0.86	 ,1
		};

		Renderer3D(GameObject* _owner);

		virtual void Render() override;

	private:

		VertexBufferLayout* layout = new VertexBufferLayout();
		VertexBuffer* vb = new VertexBuffer(mesh3D, 56 * sizeof(float));
		VertexArray* va = new VertexArray();
		IndexBuffer* ib = new IndexBuffer(indeces3D, 36);

	};
}
#endif