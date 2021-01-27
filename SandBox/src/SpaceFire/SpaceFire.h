#pragma once

#include "ShipPlayer.h"
#include "ShipEnemy.h"
#include "Core/Timer.h"
#include <random>
#include "Ammo.h"
#include "Bullet.h"
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

	SpaceFire(std::string name = "SpaceFire", int width = 800, int height = 600, bool Vsync = false) : Application(name,TYPE_2D,width,height,Vsync){}

	void OnStart() {
	/*	std::cout << NAMECOLOR << typeid(GameObject).name() << " size:" << sizeof(GameObject) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(Transform).name() << " size:" << sizeof(Transform) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(SpriteRenderer).name() << " size:" << sizeof(SpriteRenderer) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(RectangleCollider2D).name() << " size:" << sizeof(RectangleCollider2D) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(Ammo).name() << " size:" << sizeof(Ammo) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(ShipEnemy).name() << " size:" << sizeof(ShipEnemy) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(ShipPlayer).name() << " size:" << sizeof(ShipPlayer) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(Bullet).name() << " size:" << sizeof(Bullet) << RESET << std::endl;*/
		float* f = new float(5);
		uint64_t ptr = uint64_t(f);
		float f1 = *(float*)(void*)ptr;
		Renderer::s_BloomEffect = false;
		Window::GetCamera().Zoom(20);
		SoundManager::CreateSoundAsset("back", backSound);
		SoundManager::SetVolume(0.3f);
		backSound->SetVolume(0.03f);
		SoundManager::Loop(backSound);
		Gui::GetInstance()->FontBind(Gui::GetInstance()->GetStandartFonts()[Gui::GetInstance()->PEAK]);
		ImGui::SetCurrentContext(Window::s_ImGuiContext);
		ammoTexture = Texture::Create("src/SpaceFire/Images/Ammo.png");
		backgroundTexture = Texture::Create("src/SpaceFire/Images/SpaceBack.png");
		background1 = new GameObject("BackGround1");
		background1->GetComponentManager()->GetComponent<Transform>()->Scale(100, 100);
		(background1->GetComponentManager()->AddComponent<SpriteRenderer>())->m_Texture = (backgroundTexture);
		background2 = new GameObject("BackGround2");
		background2->GetComponentManager()->GetComponent<Transform>()->Scale(100, 100);
		(background2->GetComponentManager()->AddComponent<SpriteRenderer>())->m_Texture = (backgroundTexture);
		pl = (new GameObject())->AddComponent<ShipPlayer>();
		pl->Init();
		float x = -20;
		for (unsigned int i = 0; i < 5; i++)
		{
			enemies.push_back((new GameObject("ShipEnemy", x, 20))->AddComponent<ShipEnemy>());
			enemies.back()->Init();
			enemies.back()->pl = pl;
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
		Gui::GetInstance()->m_RelatedPanelProperties.m_AutoAllignment = true;
		Gui::GetInstance()->m_RelatedPanelProperties.m_Padding.y = 20;
		Gui::GetInstance()->m_RelatedPanelProperties.m_Margin = glm::vec2(50,40);
		Gui::GetInstance()->Panel("##1",x, y, 450, 200, COLORS::DarkGray * 0.8f);
		if (Gui::GetInstance()->Button("Exit", 0, 0, 30, 350, 50, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
			Window::Exit();
		}
		if (Gui::GetInstance()->Button("Restart", 0, 0, 30, 350, 50, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
			pl->Respawn();
			pause = false;
			spawnTimer = 16;
		}
		Gui::GetInstance()->m_RelatedPanelProperties.m_AutoAllignment = false;
		Gui::GetInstance()->UnRelateToPanel();
		if (Input::IsKeyPressed(Keycode::KEY_ENTER)) {
			pl->Respawn();
		}
	}

	void OnGuiRender() {
		Gui::GetInstance()->m_RelatedPanelProperties.m_Margin.x = 0;
		Gui::GetInstance()->m_RelatedPanelProperties.m_Margin.y = 0;
		for (size_t i = 0; i < enemies.size(); i++)
		{
			if (enemies[i]->isDead)
				continue;
			glm::dvec2 pos = ViewPort::GetInstance()->GetFromWorldToScreenSpace((enemies[i]->GetOwnerOfComponent()->GetPosition().x - enemies[i]->GetOwnerOfComponent()->GetComponent<SpriteRenderer>()->GetWidth() / 2.0), (enemies[i]->GetOwnerOfComponent()->GetPosition().y + enemies[i]->GetOwnerOfComponent()->GetComponent<SpriteRenderer>()->GetHeight() / 2.0));
			glm::vec2 size = ViewPort::GetInstance()->GetFromWorldToScreenSpace((enemies[i]->GetOwnerOfComponent()->GetComponent<SpriteRenderer>())->GetWidth(), 0);
		
			Gui::GetInstance()->Bar(pos.x, pos.y + 30, enemies[i]->hp, 100, COLORS::Red, COLORS::DarkGray, size.x,25 / (Window::GetCamera().GetZoomLevel() / 10));
		}

		if (pl->isDead || pause) {
			Menu();
		}
		else {
			Debug();
			//Gui::GetInstance()->Text("HP %f", true,1350, -800, fontSize, COLORS::White, 0, pl->hp);
			Gui::GetInstance()->Bar(g_Width * 0.7, -g_Height / Window::GetCamera().GetAspectRatio() * 0.8, pl->hp, 100, COLORS::Red, COLORS::DarkGray, 200, 25);
			Gui::GetInstance()->Text("Kills %d", true, g_Width * 0.7, -g_Height / Window::GetCamera().GetAspectRatio() * 0.9, fontSize, COLORS::White, 0, pl->kills);
			Gui::GetInstance()->m_YAlign = Gui::AlignVertically::YCENTER;
			Gui::GetInstance()->Text("%d", true, -g_Width * 0.85, -g_Height / Window::GetCamera().GetAspectRatio() * 0.8, fontSize, COLORS::White, 0, pl->ammo);
			Gui::GetInstance()->m_YAlign = Gui::AlignVertically::TOP;
			Gui::GetInstance()->Image(-g_Width * 0.9, -g_Height / Window::GetCamera().GetAspectRatio() * 0.8, 100, 100, ammoTexture, COLORS::White);
		}

		if (Input::IsKeyPressed(Keycode::KEY_ESCAPE) && !pl->isDead) {
			pause = !pause;
		}
	}

	void OnUpdate() {
		EventSystem::GetInstance()->StopProcessEvents(pause);
		if (background1->GetPosition().y <= -100)
			background1->GetComponentManager()->GetComponent<Transform>()->Translate(0, 100);
		if (background2->GetPosition().y <= -100)
			background2->GetComponentManager()->GetComponent<Transform>()->Translate(0, 100);
		if (!pause){
			background1->GetComponentManager()->GetComponent<Transform>()->Move(0, -100 * DeltaTime::s_Deltatime, 0);
			background2->GetComponentManager()->GetComponent<Transform>()->Move(0, -100 * DeltaTime::s_Deltatime, 0);
		}

		if (spawnTimer > 15) {
			spawnTimer = 0;
			for (unsigned int i = 0; i < enemies.size(); i++)
			{
				enemies[i]->Spawn();
			}
		}

		textRenderTimer += DeltaTime::s_Deltatime;
		spawnTimer += DeltaTime::s_Deltatime;
	}

	void OnClose() {
	}

	void Debug() {
		fps = 1000.f / (DeltaTime::s_Deltatime * 1000.f);
		Gui::GetInstance()->m_XAlign = Gui::AlignHorizontally::LEFT;
		Gui::GetInstance()->RelateToPanel();
		Gui::GetInstance()->m_RelatedPanelProperties.m_AutoAllignment = true;
		Gui::GetInstance()->m_RelatedPanelProperties.m_Padding.y = 10;
		Gui::GetInstance()->Panel("##2",g_Width * 0.5, g_Height / Window::GetCamera().GetAspectRatio() * 0.5, 450, 200, glm::vec4(1,1,1,0));
		Gui::GetInstance()->Text("FPS : %f", true, 0, 0, fontSize, COLORS::White, 0, fps);
		Gui::GetInstance()->Text("Mouse X : %f Y : %f", true, 0, 0, fontSize, COLORS::White, 1, ViewPort::GetInstance()->GetMousePositionToScreenSpace().x, ViewPort::GetInstance()->GetMousePositionToScreenSpace().y);
		Gui::GetInstance()->Text("Camera X : %f Y : %f", true, 0, 0, fontSize, COLORS::White, 1, Window::GetCamera().GetPosition().x, Window::GetCamera().GetPosition().y);
		Gui::GetInstance()->Text("Player X : %f Y : %f", true, 0, 0, fontSize, COLORS::White, 1, pl->GetOwnerOfComponent()->GetPosition().x, pl->GetOwnerOfComponent()->GetPosition().y);
		Gui::GetInstance()->m_XAlign = Gui::AlignHorizontally::LEFT;
		Gui::GetInstance()->m_RelatedPanelProperties.m_AutoAllignment = false;
		Gui::GetInstance()->UnRelateToPanel();
	}
};