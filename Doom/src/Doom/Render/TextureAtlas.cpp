#include "TextureAtlas.h"

Doom::TextureAtlas::TextureAtlas(float spriteWidth, float spriteHeight,std::string texturePath) : spriteHeight(spriteHeight),spriteWidth(spriteWidth)
{
	m_texture = new Texture(texturePath);
	textureAtlases.push_back(this);
}

Doom::TextureAtlas::TextureAtlas(float spriteWidth, float spriteHeight, Texture * texture) : spriteHeight(spriteHeight), spriteWidth(spriteWidth)
{
	m_texture = texture;
	textureAtlases.push_back(this);
}

Doom::TextureAtlas::~TextureAtlas()
{
	delete m_texture;
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

const char** Doom::TextureAtlas::GetTextureAtlases() {
	if (items != nullptr)
		delete[] items;
	items = new const char*[textureAtlases.size()];
	for (unsigned int i = 0; i < textureAtlases.size(); i++)
	{
		items[i] = textureAtlases[i]->m_texture->GetFilePath().c_str();
	}
	return items;
}