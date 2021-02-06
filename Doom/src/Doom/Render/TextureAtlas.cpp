#include "TextureAtlas.h"

Doom::TextureAtlas::TextureAtlas(std::string name, float spriteWidth, float spriteHeight, Texture * texture) : m_Name(name), m_SpriteHeight(spriteHeight), m_SpriteWidth(spriteWidth)
{
	m_Texture = texture;
	s_TextureAtlases.insert(std::make_pair(name,this));
}

Doom::TextureAtlas::~TextureAtlas()
{
}

Doom::TextureAtlas * Doom::TextureAtlas::CreateTextureAtlas(std::string name, float spriteWidth, float spriteHeight, Texture * texture)
{
	TextureAtlas* ta = GetTextureAtlas(name);
	if (ta == nullptr) return new TextureAtlas(name, spriteWidth, spriteHeight, texture);
	else return ta;
}

float * Doom::TextureAtlas::GetSpriteUVs(float x, float y)
{
	m_UVs[0] = (x * m_SpriteWidth) / m_Texture->m_width;
	m_UVs[1] = (y * m_SpriteHeight) / m_Texture->m_height;
	m_UVs[2] = ((x + 1) * m_SpriteWidth) / m_Texture->m_width;
	m_UVs[3] = (y * m_SpriteHeight) / m_Texture->m_height;
	m_UVs[4] = ((x + 1) * m_SpriteWidth) / m_Texture->m_width;
	m_UVs[5] = ((y + 1) * m_SpriteHeight) / m_Texture->m_height;
	m_UVs[6] = (x * m_SpriteWidth) / m_Texture->m_width;
	m_UVs[7] = ((y + 1) * m_SpriteHeight) / m_Texture->m_height;
	return m_UVs;
}

Doom::TextureAtlas * Doom::TextureAtlas::GetTextureAtlas(std::string name)
{
	auto iter = s_TextureAtlases.find(name);
	if (iter != s_TextureAtlases.end()) return iter->second;
	else return nullptr;
}

Doom::TextureAtlas * Doom::TextureAtlas::GetTextureAtlas(int id)
{
	int count = 0;
	for (auto iter = s_TextureAtlases.begin(); iter != s_TextureAtlases.end(); iter++) 
	{
		if (count == id) return iter->second;
		count++;
	}
	return nullptr;
}

const char** Doom::TextureAtlas::GetTextureAtlases() 
{
	if (s_NamesOfTextureAtlases != nullptr)
		delete[] s_NamesOfTextureAtlases;
	s_NamesOfTextureAtlases = new const char*[s_TextureAtlases.size()];
	uint32_t count = 0;
	for (auto iter = s_TextureAtlases.begin(); iter != s_TextureAtlases.end(); iter++) 
	{
		s_NamesOfTextureAtlases[count] = iter->second->m_Name.c_str();
		count++;
	}
	return s_NamesOfTextureAtlases;
}