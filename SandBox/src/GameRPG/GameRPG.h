#pragma once
#include "Render/TextureAtlas.h"

class GamePRG : public Application {

public:

	TextureAtlas* textureAtlas = nullptr;
	

	virtual void OnStart() override {
		textureAtlas = new TextureAtlas(128,128,"src/GameRPG/Textures/RPGpack_sheet_2X.png");

	}

	virtual void OnUpdate() override {

	}

	virtual void OnClose() override {

	}

};