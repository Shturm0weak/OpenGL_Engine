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
#include "../Render/Mesh.h"

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

		struct Material {
			float ambient = 0.2f;
			float specular = 0.2f;
		};

		bool useNormalMap = false;
		Texture* diffuseTexture = Texture::WhiteTexture;
		Texture* normalMapTexture = nullptr;

		Transform* tr = nullptr;

		void LoadMesh(Mesh* mesh);

		Shader* shader = nullptr;
		Mesh* mesh = nullptr;

		Renderer3D(GameObject* _owner);
		~Renderer3D();

		Material mat;

		virtual void Render() override;

	private:	
		VertexBufferLayout* layout =nullptr;
		VertexBuffer* vb           =nullptr;
		VertexArray* va			   =nullptr;
		IndexBuffer* ib			   =nullptr;

	};
}
#endif