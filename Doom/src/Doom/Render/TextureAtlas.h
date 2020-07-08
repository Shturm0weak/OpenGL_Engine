#pragma once
#include "../Core/Core.h"
#include "Texture.h"

namespace Doom {

	class DOOM_API TextureAtlas {
		Texture* m_texture = nullptr;
		
		float UVs[8];
		static const char** items;
	public:
		float spriteWidth; float spriteHeight;
		static std::vector<TextureAtlas*> textureAtlases;

		TextureAtlas(float spriteWidth,float spriteHeight,std::string texturePath);
		TextureAtlas(float spriteWidth, float spriteHeight, Texture* texture);
		~TextureAtlas();

		float* GetSpriteUVs(float x, float y);

		inline Texture* GetTexture() const { return m_texture; }

		inline unsigned int GetSpriteHeight() const { return spriteHeight; }

		inline unsigned int GetSpriteWidth() const { return spriteWidth; }

		static const char** GetTextureAtlases();

		void SetSpriteSize(float spriteWidth, float spriteHeight) { this->spriteHeight = spriteHeight; this->spriteWidth = spriteWidth; }

		friend class Renderer;
	};

}