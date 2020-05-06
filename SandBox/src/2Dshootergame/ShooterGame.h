#pragma once
#include "PlayerCharacter.h"

class ShooterGame : public Application {

	PlayerCharacter* pl = nullptr;

	virtual void OnStart() override {
		Renderer::Load("C:/Users/Alexandr/Desktop/ShooterGameSave.txt");
		pl = new PlayerCharacter("Player",0,0);
	}

	virtual void OnUpdate() override {
		EventSystem::Instance()->SendEvent("OnUpdate", nullptr);
		EventSystem::Instance()->ProcessEvents();
		Window::GetCamera().SetPosition(glm::vec3(pl->GetPositions().x,pl->GetPositions().y,0));
	}
	
	virtual void OnClose() override  {
		Renderer::Save("C:/Users/Alexandr/Desktop/ShooterGameSave.txt");
	}
};