#pragma once

#include "../Core/Core.h"

struct DOOM_API Mesh {
public:
	double* verteces = nullptr;
	uint32_t vertecesSize = 0;
	uint32_t* indices = nullptr;
	uint32_t indicesSize = 0;

	~Mesh() {
		delete[] verteces;
		delete indices;
	}
};