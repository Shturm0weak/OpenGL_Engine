#pragma once

#include "ShipPlayer.h"
#include "ShipEnemy.h"
#include "Core/Timer.h"

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
	void OnStart() {
			SoundManager::CreateSoundAsset("back", backSound);
			backSound->SetVolume(0.03f);
			SoundManager::Loop(backSound);
			Gui::GetInstance()->FontBind(Gui::GetInstance()->GetStandartFonts()[Gui::GetInstance()->PEAK]);
			ImGui::SetCurrentContext(Window::imGuiContext);
			ammoTexture = new Texture("src/SpaceFire/Images/Ammo.png");
			backgroundTexture = new Texture("src/SpaceFire/Images/SpaceBack.png");
			background1 = new GameObject("BackGround1");
			background1->GetComponentManager()->GetComponent<Transform>()->Scale(100, 100);
			background1->SetTexture(backgroundTexture);
			background2 = new GameObject("BackGround2");
			background2->GetComponentManager()->GetComponent<Transform>()->Scale(100, 100);
			background2->SetTexture(backgroundTexture);
			pl = new ShipPlayer();
			float x = -10;
			for (unsigned int i = 0; i < 3; i++)
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
		double x = -700 * 0.5;
		double y = 400 * 0.5;

		Gui::GetInstance()->Panel(x, y, 700, 400, COLORS::DarkGray * 0.8f);
		if (Gui::GetInstance()->Button("Exit", x - x - 250, y - 200 - 10, 40, 500, 100, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
			Window::Exit();
		}
		if (Gui::GetInstance()->Button("Restart", x - x - 250, y - 100 + 10, 40, 500, 100, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
			pl->Respawn();
			pause = false;
		}
	}

	void OnUpdate() {
		EventSystem::Instance()->SendEvent("OnUpdate", nullptr);
		EventSystem::Instance()->ProcessEvents();
		EventSystem::Instance()->StopProcessEvents(pause);
		if (background1->GetPositions().y <= -100)
			background1->GetComponentManager()->GetComponent<Transform>()->Translate(0, 100);
		if (background2->GetPositions().y <= -100)
			background2->GetComponentManager()->GetComponent<Transform>()->Translate(0, 100);
		if (!pause){
			background1->GetComponentManager()->GetComponent<Transform>()->Move(0, -100 * DeltaTime::deltatime, 0);
			background2->GetComponentManager()->GetComponent<Transform>()->Move(0, -100 * DeltaTime::deltatime, 0);
		}

		if (spawnTimer > 10) {
			spawnTimer = 0;
			for (unsigned int i = 0; i < enemies.size(); i++)
			{
				if (enemies[i]->isDead)
					enemies[i]->Spawn();
			}
		}

		Gui::GetInstance()->Begin();
		if (pl->isDead || pause) {
			Menu();
		}
		else {
			Debug();
			Gui::GetInstance()->Text("HP %f", true, Window::GetProps()[0] * 0.7, -Window::GetProps()[1] * 0.8, fontSize, COLORS::White, 0, pl->hp);
			Gui::GetInstance()->Text("Kills %d", true, Window::GetProps()[0] * 0.7, -Window::GetProps()[1] * 0.8 + 100, fontSize, COLORS::White, 0, pl->kills);
			Gui::GetInstance()->Text("%d", true, -Window::GetProps()[0] * 0.8, -Window::GetProps()[1] * 0.8 - 25, fontSize, COLORS::White, 0, pl->ammo);
			Gui::GetInstance()->Panel(-Window::GetProps()[0] * 0.8 - 10 - 100, -Window::GetProps()[1] * 0.8,100,100,COLORS::White,ammoTexture);
		}

		if (Input::IsKeyPressed(Keycode::KEY_ESCAPE) && !pl->isDead) {
			pause = !pause;
		}
		Gui::GetInstance()->End();

		textRenderTimer += DeltaTime::deltatime;
		spawnTimer += DeltaTime::deltatime;
	}

	void OnClose() {
		delete pl;
		delete background1;
		delete background2;
	}

	void Debug() {
		fps = 1000.f / (DeltaTime::deltatime * 1000.f);
		Gui::GetInstance()->xAlign = Gui::AlignHorizontally::LEFT;
		Gui::GetInstance()->Text("FPS : %f", true, 400, 900, fontSize, COLORS::White, 0, fps);
		Gui::GetInstance()->Text("Mouse X : %f Y : %f", true, 400, 820, fontSize, COLORS::White, 1, ViewPort::Instance()->GetMousePositionToWorldSpace().x, ViewPort::Instance()->GetMousePositionToWorldSpace().y);
		Gui::GetInstance()->Text("Camera X : %f Y : %f", true, 400, 740, fontSize, COLORS::White, 1, Window::GetCamera().GetPosition().x, Window::GetCamera().GetPosition().y);
		Gui::GetInstance()->Text("Player X : %f Y : %f", true, 400, 660, fontSize, COLORS::White, 1, pl->GetPositions().x, pl->GetPositions().y);
		Gui::GetInstance()->Text("Collisions: %d", true, 400, 580, fontSize, COLORS::White, 0, Renderer::GetAmountOfCollisions());
		Gui::GetInstance()->Text("Textures: %d", true, 400, 500, fontSize, COLORS::White, 0, Texture::bindedAmount);
		Gui::GetInstance()->Text("VRAM used: %f MB", true, 400, 420, fontSize, COLORS::White, 3, Texture::VRAMused);
		Gui::GetInstance()->xAlign = Gui::AlignHorizontally::LEFT;
	}
};