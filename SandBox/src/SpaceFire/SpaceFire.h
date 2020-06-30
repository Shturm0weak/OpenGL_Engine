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
	int fontSize = 20;
	Sound* backSound = new Sound("src/SpaceFire/Sounds/back.ogg");
public:

	SpaceFire(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name,TYPE_2D,width,height,Vsync){}

	void OnStart() {
		Window::GetCamera().Zoom(20);
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

		Gui::GetInstance()->RelateToPanel();
		Gui::GetInstance()->relatedPanelProperties.autoAllignment = true;
		Gui::GetInstance()->relatedPanelProperties.padding.y = 20;
		Gui::GetInstance()->relatedPanelProperties.margin = glm::vec2(50,40);
		Gui::GetInstance()->Panel(x, y, 450, 200, COLORS::DarkGray * 0.8f);
		if (Gui::GetInstance()->Button("Exit", 0, 0, 30, 350, 50, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
			Window::Exit();
		}
		if (Gui::GetInstance()->Button("Restart", 0, 0, 30, 350, 50, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
			pl->Respawn();
			pause = false;
			spawnTimer = 16;
		}
		Gui::GetInstance()->relatedPanelProperties.autoAllignment = false;
		Gui::GetInstance()->UnRelateToPanel();
		if (Input::IsKeyPressed(Keycode::KEY_ENTER)) {
			pl->Respawn();
		}
	}

	void OnGuiRender() {

		for (size_t i = 0; i < enemies.size(); i++)
		{
			if (enemies[i]->isDead)
				continue;
			glm::dvec2 pos = ViewPort::GetInstance()->GetFromWorldToScreenSpace((enemies[i]->position.x - static_cast<SpriteRenderer*>(enemies[i]->GetComponentManager()->GetComponent<Irenderer>())->GetWidth() / 2.), (enemies[i]->position.y + static_cast<SpriteRenderer*>(enemies[i]->GetComponentManager()->GetComponent<Irenderer>())->GetHeight()));
			glm::vec2 size = ViewPort::GetInstance()->GetFromWorldToScreenSpace(static_cast<SpriteRenderer*>(enemies[i]->GetComponentManager()->GetComponent<Irenderer>())->GetWidth(), 0);
			Gui::GetInstance()->Bar(pos.x,pos.y,enemies[i]->hp,100,COLORS::Red,COLORS::DarkGray, size.x,25 / (Window::GetCamera().GetZoomLevel() / 10));
		}

		if (pl->isDead || pause) {
			Menu();
		}
		else {
			Debug();
			//Gui::GetInstance()->Text("HP %f", true,1350, -800, fontSize, COLORS::White, 0, pl->hp);
			Gui::GetInstance()->Bar(WIDTH * 0.7, -HEIGHT / Window::GetCamera().GetAspectRatio() * 0.8, pl->hp, 100, COLORS::Red, COLORS::DarkGray, 200, 25);
			Gui::GetInstance()->Text("Kills %d", true, WIDTH * 0.7, -HEIGHT / Window::GetCamera().GetAspectRatio() * 0.9, fontSize, COLORS::White, 0, pl->kills);
			Gui::GetInstance()->yAlign = Gui::AlignVertically::YCENTER;
			Gui::GetInstance()->Text("%d", true, -WIDTH * 0.85, -HEIGHT / Window::GetCamera().GetAspectRatio() * 0.8, fontSize, COLORS::White, 0, pl->ammo);
			Gui::GetInstance()->yAlign = Gui::AlignVertically::TOP;
			Gui::GetInstance()->Image(-WIDTH * 0.9, -HEIGHT / Window::GetCamera().GetAspectRatio() * 0.8, 100, 100, ammoTexture, COLORS::White);
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
		Gui::GetInstance()->RelateToPanel();
		Gui::GetInstance()->relatedPanelProperties.autoAllignment = true;
		Gui::GetInstance()->relatedPanelProperties.padding.y = 10;
		Gui::GetInstance()->Panel(WIDTH * 0.5, HEIGHT / Window::GetCamera().GetAspectRatio() * 0.5, 450, 200, glm::vec4(1,1,1,0));
		Gui::GetInstance()->Text("FPS : %f", true, 0, 0, fontSize, COLORS::White, 0, fps);
		Gui::GetInstance()->Text("Mouse X : %f Y : %f", true, 0, 0, fontSize, COLORS::White, 1, ViewPort::GetInstance()->GetMousePositionToScreenSpace().x, ViewPort::GetInstance()->GetMousePositionToScreenSpace().y);
		Gui::GetInstance()->Text("Camera X : %f Y : %f", true, 0, 0, fontSize, COLORS::White, 1, Window::GetCamera().GetPosition().x, Window::GetCamera().GetPosition().y);
		Gui::GetInstance()->Text("Player X : %f Y : %f", true, 0, 0, fontSize, COLORS::White, 1, pl->GetPositions().x, pl->GetPositions().y);
		Gui::GetInstance()->Text("Collisions: %d", true, 0, 0, fontSize, COLORS::White, 0, Renderer::GetAmountOfCollisions());
		Gui::GetInstance()->Text("Textures: %d", true, 0, 0, fontSize, COLORS::White, 0, Texture::bindedAmount);
		Gui::GetInstance()->Text("VRAM used: %f MB", true, 0, 0, fontSize, COLORS::White, 3, Texture::VRAMused);
		Gui::GetInstance()->xAlign = Gui::AlignHorizontally::LEFT;
		Gui::GetInstance()->relatedPanelProperties.autoAllignment = false;
		Gui::GetInstance()->UnRelateToPanel();
	}
};