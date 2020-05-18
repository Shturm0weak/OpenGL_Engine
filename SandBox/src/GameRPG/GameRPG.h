#pragma once
#include "Render/TileMap.h"

class PlayerRPG: public GameObject {
public:
	Collision* col = nullptr;
	Transform* tr = nullptr;
	std::mutex mtx;
	PlayerRPG(const std::string name, float x, float y) :GameObject(name, x, y) {
		EventSystem::Instance()->RegisterClient("OnUpdate", this);
		EventSystem::Instance()->RegisterClient("OnStart", this);
		EventSystem::Instance()->RegisterClient("OnCollision", (Listener*)this);
		EventSystem::Instance()->RegisterClient("OnMiss",this);
		col = GetComponentManager()->AddComponent<Collision>();
		col->SetTag("Player");
		tr = GetComponentManager()->GetComponent<Transform>();
		*Renderer2DLayer::type = "Player";
		tr->Scale(3, 4);

	}

	virtual void OnUpdate() override {
		//ThreadPool::Instance()->enqueue([=] {
		//	mtx.lock();
			tr->Move(0, -9.8, 0);
			col->IsCollidedSAT();
		//	mtx.unlock();
		//});
		SetColor(COLORS::White);		
		//tr->Translate(Window::GetMousePositionToWorldSpace().x, Window::GetMousePositionToWorldSpace().y);
	}

	virtual void OnCollision(void* _col) override {
		SetColor(COLORS::Red);
	}
};

class GamePRG : public Application {

public:
	GameObject* obstacle = nullptr;
	TextureAtlas* textureAtlas = nullptr;
	PlayerRPG* pl = nullptr;
	GameObject* go = nullptr;
	double timer = -1;
	int id = 0;
	Font* font = nullptr;
	virtual void OnStart() override {
		obstacle = new GameObject("obstacle", 0, -10);
		obstacle->GetComponentManager()->AddComponent<Collision>()->IsTrigger = false;
		pl = new PlayerRPG("Player",0,0);
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
		font = new Font();
		font->LoadFont("src/font.txt", "src/arial.png");
		font->LoadCharacters();
	}

	virtual void OnUpdate() override {
		EventSystem::Instance()->SendEvent("OnUpdate", nullptr);
		EventSystem::Instance()->StopProcessEvents(false);
		if (timer > 0.2 && Input::IsMousePressed(GLFW_MOUSE_BUTTON_1)) {
			timer = 0;
			Renderer::SelectObject();
		}
		
		timer += DeltaTime::deltatime;

		Batch::GetInstance()->indexcount = 0;
		Batch::GetInstance()->Begin();
		Gui::GetInstance()->Text(font, "Mouse X : %f   Y : %f", true, 30, 36, 22, COLORS::White, 2, Window::GetMousePositionToWorldSpace().x, Window::GetMousePositionToWorldSpace().y);
		Gui::GetInstance()->Text(font, "ID : %d", true, 30, 33, 22, COLORS::White, 0, id);
		if (Editor::Instance()->selectedGO != nullptr) {
			go = Editor::Instance()->selectedGO;
			id = go->GetId();
		}
		else
			id = -1;
		Batch::GetInstance()->End();

	}

	virtual void OnClose() override {
		delete textureAtlas;
	}

};