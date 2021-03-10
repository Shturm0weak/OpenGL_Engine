#pragma once
#include "../Core/Core.h"
#include "OpenGl/Texture.h"

namespace Doom {

	class DOOM_API TextureAtlas {
	private:

		Texture* m_Texture = nullptr;
		float m_UVs[8];
		static const char** s_NamesOfTextureAtlases;

		TextureAtlas(std::string name, float spriteWidth, float spriteHeight, std::string texturePath);
		TextureAtlas(std::string name, float spriteWidth, float spriteHeight, Texture* texture);
		~TextureAtlas();
	public:

		std::string m_Name;
		float m_SpriteWidth; float m_SpriteHeight;
		static std::unordered_map<std::string,TextureAtlas*> s_TextureAtlases;

		static TextureAtlas* CreateTextureAtlas(std::string name, float spriteWidth, float spriteHeight, Texture* texture);
		static TextureAtlas* GetTextureAtlas(std::string name);
		static TextureAtlas* GetTextureAtlas(int id);
		static const char** GetTextureAtlases();
		inline Texture* GetTexture() const { return m_Texture; }
		inline unsigned int GetSpriteHeight() const { return m_SpriteHeight; }
		inline unsigned int GetSpriteWidth() const { return m_SpriteWidth; }
		float* GetSpriteUVs(float x, float y);
		void SetSpriteSize(float spriteWidth, float spriteHeight) { this->m_SpriteHeight = spriteHeight; this->m_SpriteWidth = spriteWidth; }

		friend class Renderer;
	};

}