#pragma once

class TestImGui : public Application {

	std::vector<GameObject*> go;


	virtual void OnStart() override {
		float xoffset = -3.f;
		for (size_t i = 0; i < 4; i++)
		{
			go.push_back(new GameObject());
			go[i]->GetComponentManager()->RemoveComponent<Irenderer>();
			go[i]->GetComponentManager()->AddComponent<Renderer3D>();
			go[i]->GetComponentManager()->GetComponent<Transform>()->Translate(xoffset, 0, 0);
			xoffset += 3;
		}
	
	}

	virtual void OnGuiRender() {
		Gui::GetInstance()->xAlign = Gui::GetInstance()->XCENTER;
		Gui::GetInstance()->Text("It seems I got 3D Cubes",true,0,0,76);
		Gui::GetInstance()->xAlign = Gui::GetInstance()->LEFT;
	}

	virtual void OnUpdate() override {

		if (Input::IsKeyPressed(Keycode::SPACE)) {
			Window::GetCamera().SetOrthographic(-16.f, 16.f, 9.f, -9.f, -10, 10);
		}
		else if (Input::IsKeyPressed(Keycode::KEY_F)) {
			Window::GetCamera().SetPerspective(1,16,9,0,-10);
		}

		for (size_t i = 0; i < 3; i++)
		{
			go[i]->GetComponentManager()->GetComponent<Transform>()->Rotate(-1, glm::vec3(1, 1, 1));
		}
		
	}

	virtual void OnClose() {
	}

};