#pragma once

#include "Render/TileMap.h"
#include "Core/Timer.h"

class StressTest : public Application {
public:

	float hp = 100.0f;
	bool isHovered = false;
	bool MoveWithMouse = true;
	bool value = false;
	glm::vec2 pos;
	float radius = 0;
	float panelRadius = 0;
	TextureAtlas* textureAtlas = nullptr;
	StressTest(std::string name = "TileMap", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_2D, width, height, Vsync) {}
	void OnStart() {
		textureAtlas = TextureAtlas::CreateTextureAtlas("TileMap",128, 128, Texture::Create("src/GameRPG/Textures/RPGpack_sheet_2X.png"));
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
		textures['W'] = glm::vec2(11, 11);
		textures['G'] = glm::vec2(1, 11);
		textures['B'] = glm::vec2(3, 3);
		TileMap::LoadMap(24, 4, map, textures, textureAtlas);
		TileMap::LoadMap(24, 4, map2, textures, textureAtlas);
	}

	void OnUpdate() {
		hp -= 10.0f * DeltaTime::s_Deltatime;
		if (hp < 0)
			hp = 100;

		if (Input::IsKeyPressed(Keycode::SPACE)) {
			MoveWithMouse = !MoveWithMouse;
		}
	}

	void OnGuiRender() {
		glm::vec2 posInScreen = ViewPort::GetInstance().GetMousePositionToScreenSpace();
		if(MoveWithMouse)
			pos = posInScreen;
		Gui& g = Gui::GetInstance();
		g.FontBind(g.GetStandartFonts()[g.CALIBRI]);
		g.RelateToPanel();
		g.m_RelatedPanelProperties.m_AutoAllignment = true;
		g.m_RelatedPanelProperties.m_Margin = glm::vec2(30);
		g.m_RelatedPanelProperties.m_Padding.y = 15;
		g.Panel(L"##Properties", pos.x, pos.y, 1000, 700, glm::vec4(0.3, 0.3, 0.3, 0.8));
		if (g.Button(L"Выход", 0, 0, 20, 100, 50, COLORS::Silver, COLORS::Silver * 0.8f)) {
		Window::GetInstance().Exit();
		}
		g.Bar(0, 0, hp, 100, COLORS::Red, COLORS::Silver, 150, 25);
		isHovered = g.IsPanelHovered();
		g.CheckBox(L"Пересекает ли мышь панель?", &isHovered, 0, 0, 20, COLORS::White);
		g.Text(L"Координаты панели.", true, 0, 0, 20);
		g.Text(L"X: %f Y: %f", true, 0, 0, 20, COLORS::White, 2, pos.x, pos.y);
		g.CheckBox(L"Флажок", &value, 0, 0, 20, COLORS::White);
		g.Button(L"Бесмысленно", 0, 0, 20, 175, 50, COLORS::Silver, COLORS::Silver * 0.8f);
		g.Bar(0, 0, panelRadius, 100, COLORS::Yellow, COLORS::Silver, 150, 25);
		g.SliderFloat(L"Ползунок", &panelRadius, 0, 100, 0, 0, 200, 25);
		g.FontBind(g.GetStandartFonts()[g.PLAYBALL]);
		g.Text(L"K/C: %f", true, 0, 0, 20, COLORS::White, 2, Window::GetInstance().GetFPS());
		g.m_RelatedPanelProperties.m_AutoAllignment = false;
		g.m_RelatedPanelProperties.m_Margin = glm::vec2(0);
		g.m_RelatedPanelProperties.m_Padding.y = 0;
		g.UnRelateToPanel();
	}

 };