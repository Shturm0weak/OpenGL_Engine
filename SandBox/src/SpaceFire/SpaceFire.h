#pragma once

#include "ShipPlayer.h"
#include "ShipEnemy.h"
#include "Core/Timer.h"
#include <random>

class SpaceFire : public Application {

	ShipPlayer* pl = nullptr;
	std::vector<ShipEnemy*> enemies;
	GameObject* background1 = nullptr;
	GameObject* background2 = nullptr;
	Texture* backgroundTexture = nullptr;
	Texture* ammoTexture = nullptr;
	double spawnTimer = 0;
	double textRenderTimer = 0;
	bool pause = false;
	double fps = 0;
	int fontSize = 40;
	Sound* backSound = new Sound("src/SpaceFire/Sounds/back.ogg");
public:

	SpaceFire(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name,width,height,Vsync){}

	void OnStart() {
		SoundManager::CreateSoundAsset("back", backSound);
		SoundManager::SetVolume(0.3f);
		backSound->SetVolume(0.03f);
		SoundManager::Loop(backSound);
		Gui::GetInstance()->FontBind(Gui::GetInstance()->GetStandartFonts()[Gui::GetInstance()->PEAK]);
		ImGui::SetCurrentContext(Window::imGuiContext);
		ammoTexture = new Texture("src/SpaceFire/Images/Ammo.png");
		backgroundTexture = new Texture("src/SpaceFire/Images/SpaceBack.png");
		background1 = new GameObject("BackGround1");
		background1->GetComponentManager()->GetComponent<Transform>()->Scale(100, 100);
		static_cast<SpriteRenderer*>(background1->GetComponentManager()->GetComponent<Irenderer>())->SetTexture(backgroundTexture);
		background2 = new GameObject("BackGround2");
		background2->GetComponentManager()->GetComponent<Transform>()->Scale(100, 100);
		static_cast<SpriteRenderer*>(background2->GetComponentManager()->GetComponent<Irenderer>())->SetTexture(backgroundTexture);
		pl = new ShipPlayer();
		float x = -20;
		for (unsigned int i = 0; i < 5; i++)
		{
			enemies.push_back(new ShipEnemy("ShipEnemy", x, 20));
			enemies[i]->pl = pl;
			x += 10;
		}
		pl->hp = 0;
		background1->GetComponentManager()->GetComponent<Transform>()->Translate(0, 0);
		background2->GetComponentManager()->GetComponent<Transform>()->Translate(0, 100);
			
	}

	void Menu() {
		double x = 0;
		double y = 0;

		Gui::GetInstance()->Panel(x, y, 700, 400, COLORS::DarkGray * 0.8f);
		if (Gui::GetInstance()->Button("Exit", x + 0, y - 50 - 10, 40, 500, 100, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
			Window::Exit();
		}
		if (Gui::GetInstance()->Button("Restart", y + 0, y + 50 + 10, 40, 500, 100, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
			pl->Respawn();
			pause = false;
			spawnTimer = 16;
		}
		if (Input::IsKeyPressed(Keycode::KEY_ENTER)) {
			pl->Respawn();
		}
	}

	void OnGuiRender() {

		for (size_t i = 0; i < enemies.size(); i++)
		{
			if (enemies[i]->isDead)
				continue;
			glm::dvec2 pos = ViewPort::GetInstance()->GetFromWorldToScreenSpace((enemies[i]->position.x - Window::GetCamera().GetPosition().x), (enemies[i]->position.y + static_cast<SpriteRenderer*>(enemies[i]->GetComponentManager()->GetComponent<Irenderer>())->GetHeight() - Window::GetCamera().GetPosition().y));
			Gui::GetInstance()->Bar(pos.x,pos.y,enemies[i]->hp,100,COLORS::Red,COLORS::DarkGray,400 / Window::GetCamera().GetZoomLevel(),50 / Window::GetCamera().GetZoomLevel());
		}

		if (pl->isDead || pause) {
			Menu();
		}
		else {
			Debug();
			//Gui::GetInstance()->Text("HP %f", true,1350, -800, fontSize, COLORS::White, 0, pl->hp);
			Gui::GetInstance()->Bar(1350 + 200, -600, pl->hp, 100, COLORS::Red, COLORS::DarkGray, 400, 50);
			Gui::GetInstance()->Text("Kills %d", true, 1350, -800 + 100, fontSize, COLORS::White, 0, pl->kills);
			Gui::GetInstance()->yAlign = Gui::AlignVertically::YCENTER;
			Gui::GetInstance()->Text("%d", true, -1350, -800 - 25, fontSize, COLORS::White, 0, pl->ammo);
			Gui::GetInstance()->yAlign = Gui::AlignVertically::TOP;
			Gui::GetInstance()->Panel(-1350 - 10 - 100, -800 - 25, 100, 100, COLORS::White, ammoTexture);
		}

		if (Input::IsKeyPressed(Keycode::KEY_ESCAPE) && !pl->isDead) {
			pause = !pause;
		}
	}

	void OnUpdate() {
		EventSystem::GetInstance()->SendEvent("OnUpdate", nullptr);
		EventSystem::GetInstance()->ProcessEvents();
		EventSystem::GetInstance()->StopProcessEvents(pause);
		if (background1->GetPositions().y <= -100)
			background1->GetComponentManager()->GetComponent<Transform>()->Translate(0, 100);
		if (background2->GetPositions().y <= -100)
			background2->GetComponentManager()->GetComponent<Transform>()->Translate(0, 100);
		if (!pause){
			background1->GetComponentManager()->GetComponent<Transform>()->Move(0, -100 * DeltaTime::deltatime, 0);
			background2->GetComponentManager()->GetComponent<Transform>()->Move(0, -100 * DeltaTime::deltatime, 0);
		}

		if (spawnTimer > 15) {
			spawnTimer = 0;
			for (unsigned int i = 0; i < enemies.size(); i++)
			{
				enemies[i]->Spawn();
			}
		}

		textRenderTimer += DeltaTime::deltatime;
		spawnTimer += DeltaTime::deltatime;
	}

	void OnClose() {
	}

	void Debug() {
		fps = 1000.f / (DeltaTime::deltatime * 1000.f);
		Gui::GetInstance()->xAlign = Gui::AlignHorizontally::LEFT;
		Gui::GetInstance()->Text("FPS : %f", true, 400, 900, fontSize, COLORS::White, 0, fps);
		Gui::GetInstance()->Text("Mouse X : %f Y : %f", true, 400, 820, fontSize, COLORS::White, 1, ViewPort::GetInstance()->GetMousePositionToWorldSpace().x, ViewPort::GetInstance()->GetMousePositionToWorldSpace().y);
		Gui::GetInstance()->Text("Camera X : %f Y : %f", true, 400, 740, fontSize, COLORS::White, 1, Window::GetCamera().GetPosition().x, Window::GetCamera().GetPosition().y);
		Gui::GetInstance()->Text("Player X : %f Y : %f", true, 400, 660, fontSize, COLORS::White, 1, pl->GetPositions().x, pl->GetPositions().y);
		Gui::GetInstance()->Text("Collisions: %d", true, 400, 580, fontSize, COLORS::White, 0, Renderer::GetAmountOfCollisions());
		Gui::GetInstance()->Text("Textures: %d", true, 400, 500, fontSize, COLORS::White, 0, Texture::bindedAmount);
		Gui::GetInstance()->Text("VRAM used: %f MB", true, 400, 420, fontSize, COLORS::White, 3, Texture::VRAMused);
		Gui::GetInstance()->xAlign = Gui::AlignHorizontally::LEFT;
	}
};