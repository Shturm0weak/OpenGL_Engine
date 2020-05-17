#pragma once
#include "../Core/Core.h"

namespace Doom {

	class DOOM_API TileMap {
	public:
		static bool LoadMap(unsigned int width, unsigned int height, const char* map, std::unordered_map<char, glm::vec2>& uvs, TextureAtlas* textureAtlas);
	};

}