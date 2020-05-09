#pragma once
#include "Render/TextureAtlas.h"

class GamePRG : public Application {

public:

	TextureAtlas* textureAtlas = nullptr;
	

	virtual void OnStart() override {
		textureAtlas = new TextureAtlas(128,128,"src/GameRPG/Textures/RPGpack_sheet_2X.png");
		//std::cout << textureAtlas->GetTexture()->GetHeight() << "\n";
		//std::cout << textureAtlas->GetTexture()->GetWidth() << "\n";
	}

	virtual void OnUpdate() override {

	}

	virtual void OnClose() override {

	}

};