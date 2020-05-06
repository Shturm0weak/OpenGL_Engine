#pragma once
#include "../Core/Core.h"
#include "Texture.h"

namespace Doom {

	class DOOM_API TextureAtlas {
		Texture* m_texture = nullptr;
		
		float UVs[8];

	public:
		float spriteWidth; float spriteHeight;
		static std::vector<TextureAtlas*> textureAtlases;

		TextureAtlas(float spriteWidth,float spriteHeight,std::string texturePath);
		TextureAtlas(float spriteWidth, float spriteHeight, Texture* texture);
		~TextureAtlas();

		float* GetSpriteUVs(float x, float y);

		inline Texture* GetTexture() { return m_texture; }

		friend class Renderer;
	};

}