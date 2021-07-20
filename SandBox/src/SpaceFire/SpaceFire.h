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

	SpaceFire(std::string name = "SpaceFire", int width = 800, int height = 600, bool Vsync = false) : Application(name, width, height, Vsync){}

	void OnStart() {
	/*	std::cout << NAMECOLOR << typeid(GameObject).name() << " size:" << sizeof(GameObject) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(Transform).name() << " size:" << sizeof(Transform) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(SpriteRenderer).name() << " size:" << sizeof(SpriteRenderer) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(RectangleCollider2D).name() << " size:" << sizeof(RectangleCollider2D) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(Ammo).name() << " size:" << sizeof(Ammo) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(ShipEnemy).name() << " size:" << sizeof(ShipEnemy) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(ShipPlayer).name() << " size:" << sizeof(ShipPlayer) << RESET << std::endl;
		std::cout << NAMECOLOR << typeid(Bullet).name() << " size:" << sizeof(Bullet) << RESET << std::endl;*/
		Renderer::s_Bloom.m_IsEnabled = false;
		Camera& camera = Window::GetInstance().GetCamera();
		camera.Zoom(20);
		camera.SetOrthographic(camera.m_Ratio);
		SoundManager::GetInstance().CreateSoundAsset("back", backSound);
		SoundManager::GetInstance().SetVolume(0.3f);
		backSound->SetVolume(0.03f);
		SoundManager::GetInstance().Loop(backSound);
		Gui::GetInstance().FontBind(Gui::GetInstance().GetStandartFonts()[Gui::GetInstance().PEAK]);
		ImGui::SetCurrentContext(Window::GetInstance().s_ImGuiContext);
		ammoTexture = Texture::Create("src/SpaceFire/Images/Ammo.png");
		backgroundTexture = Texture::Create("src/SpaceFire/Images/SpaceBack.png");
		background1 = GameObject::Create("BackGround1");
		background1->m_ComponentManager.GetComponent<Transform>()->Scale(100, 100);
		(background1->m_ComponentManager.AddComponent<SpriteRenderer>())->m_Texture = (backgroundTexture);
		background2 = GameObject::Create("BackGround2");
		background2->m_ComponentManager.GetComponent<Transform>()->Scale(100, 100);
		(background2->m_ComponentManager.AddComponent<SpriteRenderer>())->m_Texture = (backgroundTexture);
		pl = (GameObject::Create())->AddComponent<ShipPlayer>();
		pl->Init();
		float x = -20;
		for (unsigned int i = 0; i < 5; i++)
		{
			enemies.push_back((GameObject::Create("ShipEnemy", x, 20))->AddComponent<ShipEnemy>());
			enemies.back()->Init();
			enemies.back()->pl = pl;
			x += 10;
		}
		pl->hp = 0;
		background1->m_ComponentManager.GetComponent<Transform>()->Translate(0, 0);
		background2->m_ComponentManager.GetComponent<Transform>()->Translate(0, 100);
			
	}

	void Menu() {
		double x = 0;
		double y = 0;
		Gui& g = Gui::GetInstance();
		g.RelateToPanel();
		g.m_RelatedPanelProperties.m_AutoAllignment = true;
		g.m_RelatedPanelProperties.m_Padding.y = 20;
		g.m_RelatedPanelProperties.m_Margin = glm::vec2(50,40);
		g.Panel(L"##1",x, y, 450, 200, COLORS::DarkGray * 0.8f);
		if (g.Button(L"Exit", 0, 0, 30, 350, 50, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
			Window::GetInstance().Exit();
		}
		if (g.Button(L"Restart", 0, 0, 30, 350, 50, COLORS::Gray * 0.7f, COLORS::Gray * 0.7f * 0.5f)) {
			pl->Respawn();
			pause = false;
			spawnTimer = 16;
		}
		g.m_RelatedPanelProperties.m_AutoAllignment = false;
		g.UnRelateToPanel();
		if (Input::IsKeyPressed(Keycode::KEY_ENTER)) {
			pl->Respawn();
		}
	}

	void OnGuiRender() {
		Camera& camera = Window::GetInstance().GetCamera();
		Gui& g = Gui::GetInstance();
		g.m_RelatedPanelProperties.m_Margin.x = 0;
		g.m_RelatedPanelProperties.m_Margin.y = 0;
		for (size_t i = 0; i < enemies.size(); i++)
		{
			if (enemies[i]->isDead)
				continue;
			glm::dvec2 pos = ViewPort::GetInstance().GetFromWorldToScreenSpace((enemies[i]->m_OwnerOfCom->GetPosition().x - enemies[i]->m_OwnerOfCom->GetComponent<SpriteRenderer>()->GetWidth() / 2.0), (enemies[i]->m_OwnerOfCom->GetPosition().y + enemies[i]->m_OwnerOfCom->GetComponent<SpriteRenderer>()->GetHeight() / 2.0));
			glm::vec2 size = ViewPort::GetInstance().GetFromWorldToScreenSpace((enemies[i]->m_OwnerOfCom->GetComponent<SpriteRenderer>())->GetWidth(), 0);
		
			g.Bar(pos.x, pos.y + 30, enemies[i]->hp, 100, COLORS::Red, COLORS::DarkGray, size.x,25 / (camera.m_ZoomScale / 10));
		}

		if (pl->isDead || pause) {
			Menu();
		}
		else {
			Debug();
			//g.Text("HP %f", true,1350, -800, fontSize, COLORS::White, 0, pl->hp);
			g.Bar(g_ScaleUI * 0.7, -g_ScaleUI / camera.m_Ratio * 0.8, pl->hp, 100, COLORS::Red, COLORS::DarkGray, 200, 25);
			g.Text(L"Kills %d", true, g_ScaleUI * 0.7, -g_ScaleUI / camera.m_Ratio * 0.9, fontSize, COLORS::White, 0, pl->kills);
			g.m_YAlign = Gui::AlignVertically::YCENTER;
			g.Text(L"%d", true, -g_ScaleUI * 0.85, -g_ScaleUI / camera.m_Ratio * 0.8, fontSize, COLORS::White, 0, pl->ammo);
			g.m_YAlign = Gui::AlignVertically::TOP;
			g.Image(-g_ScaleUI * 0.9, -g_ScaleUI / camera.m_Ratio * 0.8, 100, 100, ammoTexture, COLORS::White);
		}

		if (Input::IsKeyPressed(Keycode::KEY_ESCAPE) && !pl->isDead) {
			pause = !pause;
		}
	}

	void OnUpdate() {
		glDisable(GL_DEPTH_TEST);
		EventSystem::GetInstance().StopProcessEvents(pause);
		if (background1->GetPosition().y <= -100)
			background1->m_ComponentManager.GetComponent<Transform>()->Translate(0, 100);
		if (background2->GetPosition().y <= -100)
			background2->m_ComponentManager.GetComponent<Transform>()->Translate(0, 100);
		if (!pause){
			background1->m_ComponentManager.GetComponent<Transform>()->Move(0, -100 * DeltaTime::s_Deltatime, 0);
			background2->m_ComponentManager.GetComponent<Transform>()->Move(0, -100 * DeltaTime::s_Deltatime, 0);
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
		Camera& camera = Window::GetInstance().GetCamera();
		Gui& g = Gui::GetInstance();
		fps = 1000.f / (DeltaTime::s_Deltatime * 1000.f);
		g.m_XAlign = Gui::AlignHorizontally::LEFT;
		g.RelateToPanel();
		g.m_RelatedPanelProperties.m_AutoAllignment = true;
		g.m_RelatedPanelProperties.m_Padding.y = 10;
		g.Panel(L"##2",g_ScaleUI * 0.5, g_ScaleUI / camera.m_Ratio * 0.5, 450, 200, glm::vec4(1,1,1,0));
		g.Text(L"FPS : %f", true, 0, 0, fontSize, COLORS::White, 0, fps);
		g.Text(L"Mouse X : %f Y : %f", true, 0, 0, fontSize, COLORS::White, 1, ViewPort::GetInstance().GetMousePositionToScreenSpace().x, ViewPort::GetInstance().GetMousePositionToScreenSpace().y);
		g.Text(L"Camera X : %f Y : %f", true, 0, 0, fontSize, COLORS::White, 1, camera.GetPosition().x, camera.GetPosition().y);
		g.Text(L"Player X : %f Y : %f", true, 0, 0, fontSize, COLORS::White, 1, pl->m_OwnerOfCom->GetPosition().x, pl->m_OwnerOfCom->GetPosition().y);
		g.m_XAlign = Gui::AlignHorizontally::LEFT;
		g.m_RelatedPanelProperties.m_AutoAllignment = false;
		g.UnRelateToPanel();
	}
};