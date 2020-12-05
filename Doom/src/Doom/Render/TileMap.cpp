#include "../pch.h"
#include "TileMap.h"

using namespace Doom;

bool TileMap::LoadMap(unsigned int width, unsigned int height, const char* map, std::unordered_map<char, glm::vec2>& uvs, TextureAtlas* textureAtlas) {
	unsigned int index = 0;
	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			if (map[index] != ' ') {
				std::string name = "Tile:" + std::to_string(j) + " " + std::to_string(i);
				GameObject* go = new GameObject(name, j, height - i - 1);
				SpriteRenderer* sr = go->GetComponentManager()->AddComponent<SpriteRenderer>();
				auto textureItr = uvs.find(map[index]);
				if (textureItr != uvs.end()) {
					sr->SetTexture(textureAtlas->GetTexture());
					sr->SetUVs(textureAtlas->GetSpriteUVs(textureItr->second.x, textureItr->second.y));
				}
				else
					sr->SetTexture(Texture::s_WhiteTexture);
			}
			index++;
		}
	}
	return true;
}