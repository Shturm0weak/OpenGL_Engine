#pragma once


class TestImGui : public Application {

	std::vector<GameObject*> go;
	double t = 0;

	virtual void OnStart() override {
		float xoffset = -10.f;
		for (size_t i = 0; i < 2; i++)
		{
			go.push_back(new GameObject());
			go[i]->GetComponentManager()->RemoveComponent<Irenderer>();
			go[i]->GetComponentManager()->AddComponent<Renderer3D>();
			go[i]->GetComponentManager()->GetComponent<Transform>()->Translate(xoffset, 0, 0);
			xoffset += 10;
		}

		MeshManager::LoadMesh("plant", "src/Mesh/tree.fbx");
		MeshManager::LoadMesh("cube", "src/Mesh/cube.fbx");


		Renderer::Light->GetComponentManager()->GetComponent<Transform>()->Translate(0, 5, 0);
		Renderer::Light->GetComponentManager()->GetComponent<Transform>()->Scale(0.1, 0.1, 0.1);
		static_cast<Renderer3D*>(go[0]->GetComponentManager()->GetComponent<Irenderer>())->SetColor(COLORS::Orange);
		static_cast<Renderer3D*>(go[0]->GetComponentManager()->GetComponent<Irenderer>())->LoadMesh(MeshManager::GetMesh("plant"));
		go[0]->GetComponentManager()->GetComponent<Transform>()->Scale(0.01, 0.01, 0.01);
		Renderer::Light->GetComponentManager()->RemoveComponent<Irenderer>();
		Renderer::Light->GetComponentManager()->AddComponent<Renderer3D>();
		static_cast<Renderer3D*>(Renderer::Light->GetComponentManager()->GetComponent<Irenderer>())->SetColor(COLORS::White);
		static_cast<Renderer3D*>(Renderer::Light->GetComponentManager()->GetComponent<Irenderer>())->LoadMesh(MeshManager::GetMesh("cube"));
		static_cast<Renderer3D*>(go[1]->GetComponentManager()->GetComponent<Irenderer>())->SetColor(COLORS::Brown);
		static_cast<Renderer3D*>(go[1]->GetComponentManager()->GetComponent<Irenderer>())->LoadMesh(MeshManager::GetMesh("plant"));
	}

	virtual void OnGuiRender() {
		Gui::GetInstance()->xAlign = Gui::GetInstance()->XCENTER;
		glm::vec3 pos = Window::GetCamera().GetPosition();
		Gui::GetInstance()->Text("X %f Y %f Z %f",true,0,0,76,COLORS::White,2,pos.x,pos.y,pos.z);
		Gui::GetInstance()->xAlign = Gui::GetInstance()->LEFT;
	}

	virtual void OnUpdate() override {
		go[0]->GetComponentManager()->GetComponent<Transform>()->Translate(sin(t) * 15, 0, sin(t) * 15);
		go[0]->GetComponentManager()->GetComponent<Transform>()->Rotate(30,30,30);
		t += DeltaTime::deltatime;
		if (Input::IsKeyPressed(Keycode::KEY_BACKSPACE)) {
			Window::GetCamera().SetOrthographic(-16.f, 16.f, 9.f, -9.f, 0.1, 100000);
		}
		else if (Input::IsKeyPressed(Keycode::KEY_F)) {
			Window::GetCamera().SetPerspective(1,16,9,0.1,100000);
		}

		for (size_t i = 0; i < 1; i++)
		{
			//go[i]->GetComponentManager()->GetComponent<Transform>()->Rotate(1,1,1);
		}
		
	}

	virtual void OnClose() {
	}

};