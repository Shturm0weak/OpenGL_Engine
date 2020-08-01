#pragma once

#include "Audio/SoundManager.h"
#include "Application.h"
#include "Components/RectangleCollider2D.h"
#include "Components/Transform.h"
#include "Player.h"
#include "Coin.h"
#include "HP.h"
#include "Render/Gui.h"
#include "Bomb.h"
#include "Render/Character.h"
#include "Render/Line.h"
#include "Core/Ray2D.h"


using namespace Doom;

class Game : public Doom::Application{
	Player* go = nullptr;
	double max, lowest, fps;
	double delay;
	bool pause;
	int previndexcount;
	std::vector<Coin*> coins;
	HP* hp = nullptr;
	Font* font = nullptr;
	Texture* texturecoin = nullptr;
	Bomb* bomb = nullptr;
	double time = 0;
	Line* line = nullptr;
	Ray2D* ray = nullptr;
	double fireTimer = 0;
	double timerFadeOut = 0;
public:
	virtual void OnStart() override{
		SoundManager::SetVolume(0.1);
		Renderer::Load("src/game/savePlatformer.txt");
		go = new Player("player", -1, 10);
		max = 0, lowest = 1000000, fps = 0;
		delay = 1;
		bomb = new Bomb();
		pause = false;
		previndexcount = 0;
		hp = new HP(-14, 6);
		font = new Font();
		texturecoin = Texture::Create("src/Images/coin.png");
		font->LoadFont("src/font.txt", "src/arial.png");
		font->LoadCharacters();
		go->HpBar = hp;
		ray = new Ray2D(glm::vec2(0, 0), glm::vec2(3,3) , 10);
		ray->ignoreMask.push_back("Player");
		for (unsigned int i = 0; i < 5; i++)
		{
			coins.push_back(new Coin());
			coins[i]->GetComponentManager()->GetComponent<SpriteRenderer>()->SetTexture(texturecoin);
			coins[i]->Randomize();
		}
		Window::GetCamera().MovePosition(glm::vec3(0, 0, 0));
		Window::GetCamera().Zoom(25);
		line = new Line(glm::vec3(-5, -5, 0), glm::vec3(-5, 5, 0));
		line->width = 5.f;
		line->Enable = false;
	}

	virtual void OnUpdate() override {
		EventSystem::GetInstance()->SendEvent("OnUpdate", nullptr);
		EventSystem::GetInstance()->StopProcessEvents(pause);
		Window::GetCamera().CameraMovement();
		time += DeltaTime::deltatime;
		if (delay > 0.1) {
			if (Input::IsKeyPressed(Keycode::KEY_ESCAPE)) {
				if (pause)
					pause = false;
				else
					pause = true;
				delay = 0;
			}
		}
		delay += DeltaTime::deltatime;
		if (go->isDead) {
			pause = true;
			if (Input::IsKeyPressed(Keycode::KEY_ENTER)) {
				pause = false;
				for (unsigned int i = 0; i < 5; i++)
				{
					coins[i]->Randomize();
				}
				for (unsigned int i = 0; i < 3; i++)
				{
					hp->hearts[i]->Enable = true;
				}
				bomb->Randomize();
				go->hp = 3;
				go->scores = 0;
				go->missed = 0;
				go->isDead = false;
				go->col->Enable = true;
				go->tr->Translate(0, 8);

			}
		}
		else if (!pause) {
			{
				if (fireTimer > 2.99 && Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
					Hit hit;
					glm::vec2 direction = glm::vec2(ViewPort::GetInstance()->GetMousePositionToWorldSpace().x - go->GetPositions().x, ViewPort::GetInstance()->GetMousePositionToWorldSpace().y - go->GetPositions().y);
					Ray2D::Normilize(direction);
					if (ray->Raycast(hit, 30.f, glm::vec2(go->GetPositions().x, go->GetPositions().y), direction, ray->ignoreMask)) {
						if (hit.Object->GetTag() == "Coin") {
							Coin* coin = (Coin*)hit.Object->GetOwnerOfComponent();
							coin->OnCollision(go->col);
						}
						else if (hit.Object->GetTag() == "Bomb") {
							Bomb* bomb = (Bomb*)hit.Object->GetOwnerOfComponent();
							go->col->SetTag("Land");
							bomb->OnCollision(go->col);
							go->col->SetTag("Player");
						}
						line->Enable = true;
						line->SetStartPoint(ray->start.x, ray->start.y);
						line->SetEndPoint(hit.point.x,hit.point.y);
						fireTimer = 0;
					}
					else {
						line->Enable = true;
						line->SetStartPoint(ray->start.x, ray->start.y);
						line->SetEndPoint(ray->end.x, ray->end.y);
						fireTimer = 0;
					}
				}
				if (line->Enable) {
					timerFadeOut += DeltaTime::deltatime;
				}
				if (timerFadeOut > 0.5 && line->Enable) {
					line->Enable = false;
					timerFadeOut = 0;
				}
				fireTimer += DeltaTime::deltatime;
				if (fireTimer > 3.0)
					fireTimer = 3.0;
				fps = 1000.f / (DeltaTime::deltatime * 1000.f);
				if (fps > max)
					max = fps;
				else if (fps < lowest)
					lowest = fps;

				
			}
		}
	}

	void OnGuiRender() {
		if (go->isDead) {
			Gui::GetInstance()->xAlign = Gui::GetInstance()->XCENTER;
			Gui::GetInstance()->Text("Press Enter to restart", true, -0, 0, 76);
			Gui::GetInstance()->xAlign = Gui::GetInstance()->LEFT;
		}

		if (!pause) {
			Gui::GetInstance()->RelateToPanel();
			Gui::GetInstance()->relatedPanelProperties.autoAllignment = true;
			Gui::GetInstance()->Panel(500, 400, 450, 200, COLORS::DarkGray * 0.0f);
			Gui::GetInstance()->Text("FPS : %f", true, 0, 0, 20, COLORS::Red, 0, fps);
			Gui::GetInstance()->Text("Mouse X : %f   Y : %f", true, 0, 0, 20, COLORS::Red, 2, ViewPort::GetInstance()->GetMousePositionToWorldSpace().x, ViewPort::GetInstance()->GetMousePositionToWorldSpace().y);
			Gui::GetInstance()->Text("Camera X : %f   Y : %f", true, 0, 0, 20, COLORS::Red, 2, Window::GetCamera().GetPosition().x, Window::GetCamera().GetPosition().y);
			Gui::GetInstance()->Text("Player X : %f   Y : %f", true, 0, 0, 20, COLORS::Red, 2, go->GetPositions().x, go->GetPositions().y);
			Gui::GetInstance()->Text("Textures: %d", true, 0, 0, 20, COLORS::Red, 0, Texture::bindedAmount);
			Gui::GetInstance()->Text("Collisions: %d", true, 0, 0, 20, COLORS::Red, 0, Renderer::GetAmountOfCollisions());
			Gui::GetInstance()->Text("VRAM used: %f MB", true, 0, 0, 20, COLORS::Red, 3, Texture::VRAMused);
			Gui::GetInstance()->Text("Time since start: %f", true, 0, 0, 20, COLORS::Red, 3, time);
			Gui::GetInstance()->UnRelateToPanel();

			Gui::GetInstance()->RelateToPanel();
			Gui::GetInstance()->relatedPanelProperties.autoAllignment = true;
			Gui::GetInstance()->Panel(600, -450, 450, 200, COLORS::DarkGray * 0.0f);
			Gui::GetInstance()->Text("Score: %d", true, 0, 0, 40, COLORS::White, 0, go->scores);
			Gui::GetInstance()->Text("Missed: %d", true, 0, 0, 40, COLORS::White, 0, go->missed);
			Gui::GetInstance()->Text("Fire Reload: %f", true, 0, 0, 40, COLORS::White, 3, fireTimer);
			Gui::GetInstance()->UnRelateToPanel();
		}
		else {
			double x = 0;
			double y = 0;
			Gui::GetInstance()->Panel(x, y, 700, 400, COLORS::DarkGray * 0.8f);
			if (Gui::GetInstance()->Button("Exit", x - 250, y - 50 - 10 + 50, 40, 500, 100, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
				Window::Exit();
			}
			if (Gui::GetInstance()->Button("Restart", x - 250, y + 50 + 10 + 50, 40, 500, 100, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
				pause = false;
				for (unsigned int i = 0; i < 5; i++)
				{
					coins[i]->Randomize();
				}
				for (unsigned int i = 0; i < 3; i++)
				{
					hp->hearts[i]->Enable = true;
				}
				bomb->Randomize();
				go->hp = 3;
				go->scores = 0;
				go->missed = 0;
				go->isDead = false;
				go->col->Enable = true;
				go->tr->Translate(0, 8);
				pause = false;
			}
		}
	}

	void OnClose() {
		//Renderer::Save("src/game/savePlatformer.txt");
	}
};