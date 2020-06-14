#pragma once

class TestImGui : public Application {

	GameObject* go = nullptr;
	Texture* texture = nullptr;
	Font* font = nullptr;
	virtual void OnStart() override {
		texture = new Texture("src/SpaceFire/Images/SpaceBack.png");
		go = new GameObject();
		go->Enable = false;
		go->SetTexture(texture);
	}

	virtual void OnUpdate() override {
		//Gui::GetInstance()->FontBind(Gui::GetInstance()->GetStandartFonts()[Gui::GetInstance()->CALIBRI]);
		//Gui::GetInstance()->Begin();
		//double x = 0; double y = 0;
		//Gui::GetInstance()->Panel(x, y,700,400,COLORS::DarkGray, nullptr);
		//Gui::GetInstance()->Text("Yes", true, x - 700 * 0.5, y + 400 * 0.5, 48);
		//Gui::GetInstance()->Button("Button",0,0,48,400,200);
		//Gui::GetInstance()->Text("Mouse pos x: %f  y: %f", true,x + 20, y - 120, 48, COLORS::White, 2, ViewPort::Instance()->GetMousePositionToScreenSpace().x, ViewPort::Instance()->GetMousePositionToScreenSpace().y);
		//if (Gui::GetInstance()->Button("Button", x + 20, y - 20, 48, 200, 100,COLORS::Blue,COLORS::Blue * 0.5f)) {
		//	go->Enable = !go->Enable;
		//}
		//Gui::GetInstance()->End();
	}

};