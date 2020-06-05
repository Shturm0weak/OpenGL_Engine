#pragma once
#include "PlayerCharacter.h"

class ShooterGame : public Application {
	Font* font = nullptr;
	PlayerCharacter* pl = nullptr;
	double timer = 1;
	virtual void OnStart() override {
		Renderer::Load("C:/Users/Alexandr/Desktop/ShooterGameSave.txt");
		pl = new PlayerCharacter("Player",0,15);
		font = new Font();
		font->LoadFont("src/calibri.txt", "src/calibri.png");
		font->LoadCharacters();
		Gui::GetInstance()->FontBind(font);
	}

	virtual void OnUpdate() override {
		if (timer > 0.2 && Input::IsMousePressed(GLFW_MOUSE_BUTTON_1)) {
			timer = 0;
			Renderer::SelectObject();
		}
		timer += DeltaTime::deltatime;

		EventSystem::Instance()->SendEvent("OnUpdate", nullptr);
		EventSystem::Instance()->ProcessEvents();
		Window::GetCamera().SetPosition(glm::vec3(pl->GetPositions().x,pl->GetPositions().y,0));
		Batch::GetInstance()->indexcount = 0;
		Batch::GetInstance()->Begin();
		Gui::GetInstance()->Text("IsLanded: %d", true, 300, 460, 76, COLORS::White, 2,pl->isLanded);
		Gui::GetInstance()->Text("Distance: %f", true, 300, 380, 76, COLORS::White, 3, pl->x);
		Gui::GetInstance()->Text("Count: %d", true, 300, 300, 76, COLORS::White, 3, pl->count);
		Batch::GetInstance()->End();
	}
	
	virtual void OnClose() override  {
		Renderer::Save("C:/Users/Alexandr/Desktop/ShooterGameSave.txt");
	}
};