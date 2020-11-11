#include "TextureAtlas.h"

Doom::TextureAtlas::TextureAtlas(std::string name, float spriteWidth, float spriteHeight, Texture * texture) : name(name), spriteHeight(spriteHeight), spriteWidth(spriteWidth)
{
	m_texture = texture;
	textureAtlases.insert(std::make_pair(name,this));
}

Doom::TextureAtlas::~TextureAtlas()
{
}

Doom::TextureAtlas * Doom::TextureAtlas::CreateTextureAtlas(std::string name, float spriteWidth, float spriteHeight, Texture * texture)
{
	TextureAtlas* ta = GetTextureAtlas(name);
	if (ta == nullptr)
		new TextureAtlas(name, spriteWidth, spriteHeight, texture);
	else
		return ta;
}

float * Doom::TextureAtlas::GetSpriteUVs(float x, float y)
{
	UVs[0] = (x * spriteWidth) / m_texture->GetWidth();
	UVs[1] = (y * spriteHeight) / m_texture->GetHeight();
	UVs[2] = ((x + 1) * spriteWidth) / m_texture->GetWidth();
	UVs[3] = (y * spriteHeight) / m_texture->GetHeight();
	UVs[4] = ((x + 1) * spriteWidth) / m_texture->GetWidth();
	UVs[5] = ((y + 1) * spriteHeight) / m_texture->GetHeight();
	UVs[6] = (x * spriteWidth) / m_texture->GetWidth();
	UVs[7] = ((y + 1) * spriteHeight) / m_texture->GetHeight();
	return UVs;
}

Doom::TextureAtlas * Doom::TextureAtlas::GetTextureAtlas(std::string name)
{
	auto iter = textureAtlases.find(name);
	if (iter != textureAtlases.end()) {
		return iter->second;
	}
	else
		return nullptr;
}

Doom::TextureAtlas * Doom::TextureAtlas::GetTextureAtlas(int id)
{
	int count = 0;
	for (auto iter = textureAtlases.begin(); iter != textureAtlases.end(); iter++) {
		if (count == id)
			return iter->second;
		count++;
	}
	return nullptr;
}

const char** Doom::TextureAtlas::GetTextureAtlases() {
	if (items != nullptr)
		delete[] items;
	items = new const char*[textureAtlases.size()];
	uint32_t count = 0;
	for (auto iter = textureAtlases.begin(); iter != textureAtlases.end(); iter++) {
		items[count] = iter->second->name.c_str();
		count++;
	}
	return items;
}