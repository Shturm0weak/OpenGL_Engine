#pragma once

class TestImGui : public Application {
	bool toolOpen = true;
	float c[4] = { 1,1,1,1 };
	double timer = 1;
	int id = -1;
	ImGuiViewport* viewport;
	GameObject* go = nullptr;
	glm::vec4 color = glm::vec4(1, 1, 1, 1);
	Font* font = nullptr;
	virtual void OnStart() override {
		ImGui::SetCurrentContext(Window::imGuiContext);
		viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		font = new Font();
		font->LoadFont("src/font.txt", "src/arial.png");
		font->LoadCharacters();
	}

	virtual void OnUpdate() override {
		if (timer > 0.2 && Input::IsMousePressed(GLFW_MOUSE_BUTTON_1)) {
			timer = 0;
			Renderer::SelectObject();
		}

		timer += DeltaTime::deltatime;

		if (Editor::Instance()->selectedGO != nullptr)
			Editor::Instance()->selectedGO->SetColor(color);

		Batch::GetInstance()->indexcount = 0;
		Batch::GetInstance()->Begin();
		Gui::GetInstance()->Text(font, "ID : %d", true, 30, 33, 22, COLORS::White, 0, id);
		if (Editor::Instance()->selectedGO != nullptr) {
			go = Editor::Instance()->selectedGO;
			id = go->GetId();
		}
		else
			id = -1;
		Batch::GetInstance()->End();
	}

	virtual void OnImGuiRender() override {

		//ImGui::SetNextWindowPos(viewport->Pos);
		//ImGui::SetNextWindowSize(viewport->Size);
		//ImGui::SetNextWindowViewport(viewport->ID);
		//static ImGuiID dockspaceID = 0;
		//bool active = true;
		//	// Declare Central dockspace
		//	dockspaceID = ImGui::GetID("HUB_DockSpace");
		//	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode/*|ImGuiDockNodeFlags_NoResize*/);

		//ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
		//if (ImGui::Begin("Dockable Window"))
		//{
		//	ImGui::TextUnformatted("Test");
		//}
		//ImGui::End();
		//ImGui::DockSpaceOverViewport(viewport);
		//ImGui::Begin("TEST", &toolOpen);
		//ImGui::End();
	}

};