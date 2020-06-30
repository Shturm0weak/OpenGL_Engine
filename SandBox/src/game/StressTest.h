#pragma once

#include "Render/TileMap.h"
#include "Core/Timer.h"

class StressTest : public Application {
public:
	GameObject* gameobj = nullptr;
	float hp = 100.0f;
	bool isHovered = false;
	bool MoveWithMouse = true;
	bool value = false;
	glm::vec2 pos;
	float radius = 0;
	float panelRadius = 0;
	TextureAtlas* textureAtlas = nullptr;
	StressTest(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_2D, width, height, Vsync) {}
	void OnStart() {
		ImGui::SetCurrentContext(Window::imGuiContext);
		Gui::GetInstance()->FontBind(Gui::GetInstance()->GetStandartFonts()[Gui::GetInstance()->ARIAL]);
		textureAtlas = new TextureAtlas(128, 128, "src/GameRPG/Textures/RPGpack_sheet_2X.png");
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
		hp -= 10.0f * DeltaTime::deltatime;
		if (hp < 0)
			hp = 100;

		if (Input::IsKeyPressed(Keycode::SPACE)) {
			MoveWithMouse = !MoveWithMouse;
		}
		//std::cout << ViewPort::GetInstance()->GetMousePositionToScreenSpace().x << "	" << ViewPort::GetInstance()->GetMousePositionToScreenSpace().y << std::endl;
		//glm::vec2 pos = ViewPort::GetInstance()->GetMousePositionToWorldSpace();
		//gameobj->GetComponentManager()->GetComponent<Transform>()->Translate(pos.x, pos.y);
	}

	void OnGuiRender() {
		glm::vec2 posInScreen = ViewPort::GetInstance()->GetMousePositionToScreenSpace();
		if(MoveWithMouse)
			pos = posInScreen;

		Gui::GetInstance()->RelateToPanel();
		Gui::GetInstance()->relatedPanelProperties.autoAllignment = true;
		Gui::GetInstance()->relatedPanelProperties.margin = glm::vec2(30);
		Gui::GetInstance()->relatedPanelProperties.padding.y = 15;
		Gui::GetInstance()->edgeRadius = panelRadius;
		Gui::GetInstance()->Panel(pos.x, pos.y, 1000, 700, glm::vec4(0.3, 0.3, 0.3, 0.8));
		Gui::GetInstance()->edgeRadius = radius;
		//Gui::GetInstance()->edgeRadius = 0.0f;
		if (Gui::GetInstance()->Button("Exit", 0, 0, 20, 100, 50, COLORS::Silver, COLORS::Silver * 0.8f)) {
			Window::Exit();
		}
		Gui::GetInstance()->Bar(0, 0, hp, 100, COLORS::Red, COLORS::Silver, 150, 25);
		isHovered = Gui::GetInstance()->IsPanelHovered();
		Gui::GetInstance()->CheckBox("Does mouse intersect the panel?", &isHovered, 0, 0, 50, COLORS::White);
		Gui::GetInstance()->Text("Panel coords.", true, 0, 0, 20);
		Gui::GetInstance()->Text("X: %f Y: %f", true, 0, 0, 20, COLORS::White, 2, pos.x, pos.y);
		Gui::GetInstance()->CheckBox("Check box", &value, 0, 0, 50, COLORS::White);
		Gui::GetInstance()->Button("Pointless", 0, 0, 20, 150, 50, COLORS::Silver, COLORS::Silver * 0.8f);
		Gui::GetInstance()->Text("FPS: %f", true, 0, 0, 20, COLORS::White, 2, Window::GetFPS());
		Gui::GetInstance()->Bar(0, 0, panelRadius, 100, COLORS::Yellow, COLORS::Silver, 150, 25);
		Gui::GetInstance()->SliderFloat("Panel corners raduis", &panelRadius, 0, 100, 0, 0, 200, 25);
		Gui::GetInstance()->SliderFloat("Elements corner radius", &radius, 0, 100, 0, 0, 200, 25);
		Gui::GetInstance()->relatedPanelProperties.autoAllignment = false;
		Gui::GetInstance()->relatedPanelProperties.margin = glm::vec2(0);
		Gui::GetInstance()->relatedPanelProperties.padding.y = 0;
		Gui::GetInstance()->UnRelateToPanel();
	}
 };