#pragma once
#include "PlayerCharacter.h"

class ShooterGame : public Application {
	PlayerCharacter* pl = nullptr;
	double timer = 1;
	virtual void OnStart() override {
		pl = new PlayerCharacter("Player",0,15);
	}

	virtual void OnUpdate() override {
		if (timer > 0.2 && Input::IsMousePressed(GLFW_MOUSE_BUTTON_1)) {
			timer = 0;
			Renderer::SelectObject();
		}
		timer += DeltaTime::deltatime;

		EventSystem::GetInstance()->SendEvent(EventType::ONUPDATE, nullptr);
		EventSystem::GetInstance()->ProcessEvents();
		Window::GetCamera().SetPosition(glm::vec3(pl->GetPositions().x,pl->GetPositions().y,0));
	}
	
	virtual void OnClose() override  {
		//Renderer::Save("C:/Users/Alexandr/Desktop/ShooterGameSave.txt");
	}
};