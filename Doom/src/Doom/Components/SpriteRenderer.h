#pragma once

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


		float m_Mesh2D[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
		};

		static std::map<char*, uint64_t> s_MemoryPool;
		static std::vector<char*> s_FreeMemory;
		static unsigned int s_Indices2D[6];

		Transform* m_Tr = nullptr;
		char* m_MemoryPoolPtr = nullptr;

		void Copy(const SpriteRenderer& rhs);

		friend class Transform;
		friend class GameObject;
		friend class Batch;
		friend class Renderer;
		friend class SceneSerializer;
		friend class ComponentManager;
	public:

		Texture* m_Texture = Texture::s_WhiteTexture;
		TextureAtlas* m_TextureAtlas = nullptr;

		SpriteRenderer(const SpriteRenderer& rhs);
		SpriteRenderer();
		~SpriteRenderer();

		void operator=(const SpriteRenderer& rhs);

		void GetTransformedVertices(float* buffer); // returns 8 floats

		virtual void Render() override;
		virtual void Delete() override;

		static Component* Create();

		float GetWidth() const;
		float GetHeight() const;
		float* GetUVs() const;
		
		//Only in int size from 0 to 1 !!!
		void ReversedUvs();
		void OriginalUvs();
		void ReverseUVs();
		void SetUVs(float* uvs);
		void Setlayer(int layer);
		void SetFrontLayer();
	};
}