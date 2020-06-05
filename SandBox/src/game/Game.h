#pragma once

#include "Application.h"
#include "Components/Collision.h"
#include "Components/Transform.h"
#include "Player.h"
#include "Coin.h"
#include "HP.h"
#include "Render/Gui.h"
#include "Bomb.h"
#include "Render/Character.h"
#include "Render/Line.h"
#include "Core/Ray.h"

using namespace Doom;

class Game : public Doom::Application{

	Player* go = nullptr;
	double max, lowest, fps;
	double timer;
	double delay;
	bool pause;
	int previndexcount;
	Coin* coins = nullptr;
	HP* hp = nullptr;
	Font* font = nullptr;
	Texture* texturecoin = nullptr;
	Bomb* bomb = nullptr;
	double time = 0;
	Line* line = nullptr;
	Ray* ray = nullptr;
	double fireTimer = 0;
	double timerFadeOut = 0;
public:
	virtual void OnStart() override{
	
		Renderer::Load("src/saved.txt");
		go = new Player("player", -1, 20);
		max = 0, lowest = 1000000, fps = 0;
		timer = 5;
		delay = 1;
		bomb = new Bomb();
		pause = false;
		previndexcount = 0;
		coins = new Coin[5];
		hp = new HP(-14, 6);
		font = new Font();
		texturecoin = new Texture("src/Images/coin.png");
		font->LoadFont("src/font.txt", "src/arial.png");
		font->LoadCharacters();
		go->HpBar = hp;
		ray = new Ray(glm::vec2(0, 0), glm::vec2(3,3) , 10);
		ray->ignoreMask.push_back("Player");
		for (unsigned int i = 0; i < 5; i++)
		{
			coins[i].SetTexture(texturecoin);
			coins[i].Randomize();
		}
		Window::GetCamera().MovePosition(glm::vec3(0, 12, 0));
		Window::GetCamera().Zoom(1.5);
		line = new Line(glm::vec2(-5, -5), glm::vec2(-5, 5));
		line->width = 5.f;
	}

	virtual void OnUpdate() override {
		//line->SetEndPoint(Window::GetMousePositionToWorldSpace().x, Window::GetMousePositionToWorldSpace().y);
		//line->SetStartPoint(go->GetPositions().x,go->GetPositions().y);
		EventSystem::Instance()->SendEvent("OnUpdate", nullptr);
		EventSystem::Instance()->StopProcessEvents(pause);
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
			Batch::GetInstance()->indexcount = 0;
			Batch::GetInstance()->Begin();
			Gui::GetInstance()->Text("Press Enter to restart", true, -22, 0, 32);
			Batch::GetInstance()->End();
			if (Input::IsKeyPressed(Keycode::KEY_ENTER)) {
				pause = false;
				for (unsigned int i = 0; i < 5; i++)
				{
					coins[i].Randomize();
				}
				for (unsigned int i = 0; i < 3; i++)
				{
					hp->hearts[i].Enable = true;
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
				if (fireTimer > 2.99 && Input::IsMousePressed(GLFW_MOUSE_BUTTON_1)) {
					Hit hit;
					glm::vec2 direction = glm::vec2(ViewPort::Instance()->GetMousePositionToWorldSpace().x - go->GetPositions().x, ViewPort::Instance()->GetMousePositionToWorldSpace().y - go->GetPositions().y);
					Ray::Normilize(direction);
					if (ray->Raycast(hit, 30.f, glm::vec2(go->GetPositions().x, go->GetPositions().y), direction, ray->ignoreMask)) {
						if (hit.Object->GetTag() == "Coin") {
							Coin* coin = (Coin*)&hit.Object->GetOwnerOfComponent();
							coin->OnCollision(go->col);
						}
						else if (hit.Object->GetTag() == "Bomb") {
							Bomb* bomb = (Bomb*)&hit.Object->GetOwnerOfComponent();
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
				if (line->Enable)
					timerFadeOut += DeltaTime::deltatime;
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

				Gui::GetInstance()->Begin();
				Gui::GetInstance()->Text("FPS : %f", true, 950, 900, 76, COLORS::Red, 0, fps);
				Gui::GetInstance()->Text("Mouse X : %f   Y : %f", true, 950, 820, 76, COLORS::Red, 2, ViewPort::Instance()->GetMousePositionToWorldSpace().x, ViewPort::Instance()->GetMousePositionToWorldSpace().y);
				Gui::GetInstance()->Text("Camera X : %f   Y : %f", true, 950, 740, 76, COLORS::Red, 2, Window::GetCamera().GetPosition().x, Window::GetCamera().GetPosition().y);
				Gui::GetInstance()->Text("Player X : %f   Y : %f", true, 950, 660, 76, COLORS::Red, 2, go->GetPositions().x, go->GetPositions().y);
				Gui::GetInstance()->Text("Textures: %d", true, 950, 580, 76, COLORS::Red, 0, Texture::bindedAmount);
				Gui::GetInstance()->Text("Score: %d", true, 1300, -820, 76, COLORS::White, 0, go->scores);
				Gui::GetInstance()->Text("Missed: %d", true, 1300, -900, 76, COLORS::White, 0, go->missed);
				Gui::GetInstance()->Text("Collisions: %d", true, 950, 500, 76, COLORS::Red, 0, Renderer::GetAmountOfCollisions());
				Gui::GetInstance()->Text("VRAM used: %f MB", true, 950, 420, 76, COLORS::Red, 3, Texture::VRAMused);
				Gui::GetInstance()->Text("Time since start: %f", true, 950, 340, 76, COLORS::Red, 3, time);
				Gui::GetInstance()->Text("Fire Reload: %f", true, -1800, -900, 76, COLORS::White, 3, fireTimer);
				Gui::GetInstance()->End();
			}
		}
		else {
			if (timer > 0.05) {
				Batch::GetInstance()->indexcount = 0;
				Batch::GetInstance()->Begin();
				/*Button button1(font, "Pause", -5, -5, 2, 1, 22, 2);
				if (button1.IsPressed()) {
					if (Input::IsMousePressed(GLFW_MOUSE_BUTTON_1)) {
						glfwSetWindowShouldClose(Window::GetWindow(), GLFW_TRUE);
					}
				}*/
				timer = 0;
				Batch::GetInstance()->End();
			}
		}
		timer += DeltaTime::deltatime;
	}

	void OnClose() {
		delete[] coins;
		delete hp;
		delete go;
	}
};