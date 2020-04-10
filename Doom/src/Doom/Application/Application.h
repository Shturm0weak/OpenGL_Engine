#pragma once

#include "Components/Collision.h"
#include "Components/Transform.h"
#include "Player.h"
#include "Coin.h"
#include "HP.h"
#include "Render/Text.h"
#include "Gui/Button.h"
#include "Bomb.h"

class Application{

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
	//GameObject* cursor = nullptr;
	Bomb* bomb = nullptr;
	double time = 0;

public:
	void OnStart() {
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
		for (unsigned int i = 0; i < 5; i++)
		{
			coins[i].SetTexture(texturecoin);
			coins[i].Randomize();
		}
		Window::GetCamera().MovePosition(glm::vec3(0, 12, 0));
		Window::GetCamera().Zoom(1.5);
		//cursor = new GameObject();
		
	}

	void OnUpdate() {
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
			Text Death(font, "Press Enter to restart", true, -22, 0, 32);
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

			//auto ProcessEvents = std::bind(&EventSystem::ProcessEvents, EventSystem::Instance());
			//ThreadPool::Instance()->enqueue(ProcessEvents);
			//cursor->GetComponentManager()->GetComponent<Transform>()->Translate(Window::GetMousePositionToWorldSpace().x, Window::GetMousePositionToWorldSpace().y);
			{
				fps = 1000.f / (DeltaTime::deltatime * 1000.f);
				if (fps > max)
					max = fps;
				else if (fps < lowest)
					lowest = fps;
				if (timer > 0.1) {
					
					Batch::GetInstance()->indexcount = 0;
					Batch::GetInstance()->Begin();
					Text CurrentFPS(font, "FPS : %f", true, 30, 28, 22, COLORS::Red(), 0, fps);
					Text DrawCalls(font, "Draw calls : %d", true, 30, 31, 22, COLORS::Red(), 3, Renderer::DrawCalls);
					Text MousePosition(font, "Mouse X : %f   Y : %f", true, 30, 34, 22, COLORS::Red(), 2, Window::GetMousePositionToScreenSpace().x, Window::GetMousePositionToScreenSpace().y);
					Text CameraPosition(font, "Camera X : %f   Y : %f", true, 30, 37, 22, COLORS::Red(), 2, Window::GetCamera().GetPosition().x, Window::GetCamera().GetPosition().y);
					//Text MaxFPS(font, "Max FPS : %f", true, 0, 40, 8, COLORS::White, 6, max);
					//Text LowestFPS(font, "Lowest FPS :  %f", true, 0, 24, 8, COLORS::White, 6, lowest);
					Text Position(font, "Player X : %f   Y : %f", true, 30, 40, 22, COLORS::Red(), 2, go->GetPositions().x, go->GetPositions().y);
					Text NumberOfTextures(font, "Textures: %d", true, 30, 25, 22, COLORS::Red(), 0, Texture::bindedAmount);
					Text Score(font, "Score: %d", true, 65, -30, 22, COLORS::White(), 0, go->scores);
					Text Missed(font, "Missed: %d", true, 65, -33, 22, COLORS::White(), 0, go->missed);
					Text Objects(font, "Particles: 200 out of %d obj's", true, 30, 13, 22, COLORS::Red(), 0, Renderer2DLayer::GetAmountOfObjects());
					Text Collisions(font, "Collisions: %d", true, 30, 16, 22, COLORS::Red(), 0, Renderer2DLayer::GetAmountOfCollisions());
					Text VRAMused(font, "VRAM used: %f MB", true, 30, 22, 22, COLORS::Red(), 3, Texture::VRAMused);
					Text TimeSinceStart(font, "Time since start: %f", true, 30, 19, 22, COLORS::Red(), 3, time);
					timer = 0;
					Batch::GetInstance()->End();
				}
			}
		}
		else {
			if (timer > 0.05) {
				Batch::GetInstance()->indexcount = 0;
				Batch::GetInstance()->Begin();
				Button button1(font, "Exit", -5, -5, 2, 1, 22, 2);
				if (button1.IsPressed()) {
					if (Input::IsMousePressed(GLFW_MOUSE_BUTTON_1)) {
						glfwSetWindowShouldClose(Window::GetWindow(), GLFW_TRUE);
					}
				}
				timer = 0;
				Batch::GetInstance()->End();
			}
		}
		timer += DeltaTime::deltatime;
	}

	void OnClose() {
		delete[] coins;
	}
};