#pragma once
#include "../Core/Core.h"
#include "OpenGl/Texture.h"

namespace Doom {

	class DOOM_API TextureAtlas {
		Texture* m_texture = nullptr;
		
		float UVs[8];
		static const char** items;
		TextureAtlas(std::string name, float spriteWidth, float spriteHeight, std::string texturePath);
		TextureAtlas(std::string name, float spriteWidth, float spriteHeight, Texture* texture);
		~TextureAtlas();
	public:
		std::string name;
		float spriteWidth; float spriteHeight;
		static std::map<std::string,TextureAtlas*> textureAtlases;

		static TextureAtlas* CreateTextureAtlas(std::string name, float spriteWidth, float spriteHeight, Texture* texture);

		float* GetSpriteUVs(float x, float y);

		static TextureAtlas* GetTextureAtlas(std::string name);
		static TextureAtlas* GetTextureAtlas(int id);

		inline Texture* GetTexture() const { return m_texture; }

		inline unsigned int GetSpriteHeight() const { return spriteHeight; }

		inline unsigned int GetSpriteWidth() const { return spriteWidth; }

		static const char** GetTextureAtlases();

		void SetSpriteSize(float spriteWidth, float spriteHeight) { this->spriteHeight = spriteHeight; this->spriteWidth = spriteWidth; }

		friend class Renderer;
	};

}