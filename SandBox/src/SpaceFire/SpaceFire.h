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
public:
	void OnStart() {
		Gui::GetInstance()->FontBind(Gui::GetInstance()->GetStandartFonts()[Gui::GetInstance()->CALIBRI]);
		ImGui::SetCurrentContext(Window::imGuiContext);
		ammoTexture = new Texture("src/SpaceFire/Images/Ammo.png");
		backgroundTexture = new Texture("src/SpaceFire/Images/SpaceBack.png");
		background1 = new GameObject("BackGround1");
		background1->GetComponentManager()->GetComponent<Transform>()->Scale(100, 100);
		background1->SetTexture(backgroundTexture);
		background2 = new GameObject("BackGround2");
		background2->GetComponentManager()->GetComponent<Transform>()->Scale(100, 101);
		background2->SetTexture(backgroundTexture);
		background2->GetComponentManager()->GetComponent<Transform>()->Translate(0, 100);
		pl = new ShipPlayer();
		float x = -10;
		for (unsigned int i = 0; i < 3; i++)
		{
			enemies.push_back(new ShipEnemy("ShipEnemy", x, 20));
			enemies[i]->pl = pl;
			x += 10;
		}
		pl->hp = 0;
	}

	void Menu() {
		double x = -700 * 0.5;
		double y = 400 * 0.5;

		Gui::GetInstance()->Panel(x, y, 700, 400, COLORS::DarkGray * 0.8f);
		if (Gui::GetInstance()->Button("Exit", x - x - 150, y - 200 - 10, 48, 300, 100, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
			Window::Exit();
		}
		if (Gui::GetInstance()->Button("Restart", x - x - 150, y - 100 + 10, 48, 300, 100, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
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
			Gui::GetInstance()->Text("HP %f", true, Window::GetProps()[0] * 0.8, -Window::GetProps()[1] * 0.8, 76, COLORS::White, 0, pl->hp);
			Gui::GetInstance()->Text("Kills %d", true, Window::GetProps()[0] * 0.8, -Window::GetProps()[1] * 0.8 + 100, 76, COLORS::White, 0, pl->kills);
			Gui::GetInstance()->Text("%d", true, -Window::GetProps()[0] * 0.8, -Window::GetProps()[1] * 0.8, 76, COLORS::White, 0, pl->ammo);
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
		Gui::GetInstance()->xAlign = Gui::AlignHorizontally::XCENTER;
		Gui::GetInstance()->Text("FPS : %f", true, 900, 900, 76, COLORS::White, 0, fps);
		Gui::GetInstance()->Text("Mouse X : %f   Y : %f", true, 900, 820, 76, COLORS::White, 2, ViewPort::Instance()->GetMousePositionToWorldSpace().x, ViewPort::Instance()->GetMousePositionToWorldSpace().y);
		Gui::GetInstance()->Text("Camera X : %f   Y : %f", true, 900, 740, 76, COLORS::White, 2, Window::GetCamera().GetPosition().x, Window::GetCamera().GetPosition().y);
		Gui::GetInstance()->Text("Player X : %f   Y : %f", true, 900, 660, 76, COLORS::White, 2, pl->GetPositions().x, pl->GetPositions().y);
		Gui::GetInstance()->Text("Collisions: %d", true, 900, 580, 76, COLORS::White, 0, Renderer::GetAmountOfCollisions());
		Gui::GetInstance()->Text("Textures: %d", true, 900, 500, 76, COLORS::White, 0, Texture::bindedAmount);
		Gui::GetInstance()->Text("VRAM used: %f MB", true, 900, 420, 76, COLORS::White, 3, Texture::VRAMused);
		Gui::GetInstance()->xAlign = Gui::AlignHorizontally::LEFT;
	}
};