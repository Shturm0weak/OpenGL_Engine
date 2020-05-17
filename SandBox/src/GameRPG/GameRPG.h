#pragma once
#include "Render/TileMap.h"

class PlayerRPG: public GameObject {
public:
	Collision* col = nullptr;
	Transform* tr = nullptr;

	PlayerRPG(const std::string name, float x, float y) :GameObject(name, x, y) {
		EventSystem::Instance()->RegisterClient("OnUpdate", (GameObject*)this);
		EventSystem::Instance()->RegisterClient("OnStart", (GameObject*)this);
		EventSystem::Instance()->RegisterClient("OnCollision", (GameObject*)this);
		EventSystem::Instance()->RegisterClient("OnMiss", (GameObject*)this);
		col = GetComponentManager()->AddComponent<Collision>();
		col->SetTag("Player");
		tr = GetComponentManager()->GetComponent<Transform>();
		*Renderer2DLayer::type = "Player";
		tr->Scale(3, 4);

	}

	virtual void OnUpdate() override {
		col->IsCollidedDIAGS();
		if (col->isCollided) {
			EventSystem::Instance()->SendEvent("OnCollision", this, col->collidedObject);
			std::cout << "Collided\n";
		}
		else {
			SetColor(COLORS::White);
		}
	}

	virtual void OnCollision(void* _col) override {
		SetColor(COLORS::Red);
	}
};

class GamePRG : public Application {

public:

	TextureAtlas* textureAtlas = nullptr;
	//PlayerRPG* pl = nullptr;
	
	virtual void OnStart() override {
		//pl = new PlayerRPG("Player",0,0);
		textureAtlas = new TextureAtlas(128,128,"src/GameRPG/Textures/RPGpack_sheet_2X.png");
		std::unordered_map<char, glm::vec2> textures;
		const char* map =
			"WWWWWWWWWWWWWWWWWWWWWWWW"
			"WWWWWWWWWWGGGGGGWWWWWWWW"
			"WWWWWWWWWGGGGGGGWWWWWWWW"
			"WWWWWWWWWGWWWWWWWWWWWWWW";
		const char* map2 =
			"                        "
			"             B          "
			"            BB          "
			"                        ";
		textures['W'] = glm::vec2(11,11);
		textures['G'] = glm::vec2(1, 11);
		textures['B'] = glm::vec2(3, 3);
		TileMap::LoadMap(24, 4, map, textures, textureAtlas);
		TileMap::LoadMap(24, 4, map2, textures, textureAtlas);
	}

	virtual void OnUpdate() override {
		EventSystem::Instance()->SendEvent("OnUpdate", nullptr);
		EventSystem::Instance()->StopProcessEvents(false);
		//pl->tr->Translate(Window::GetMousePositionToWorldSpace().x, Window::GetMousePositionToWorldSpace().y);
	}

	virtual void OnClose() override {
		delete textureAtlas;
	}

};