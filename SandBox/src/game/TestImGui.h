#pragma once

#include "FbxLoader/fbxdocument.h"

class TestImGui : public Application {

	std::vector<GameObject*> go;
	fbx::FBXDocument doc;

	virtual void OnStart() override {
		float xoffset = -10.f;
		for (size_t i = 0; i < 3; i++)
		{
			go.push_back(new GameObject());
			go[i]->GetComponentManager()->RemoveComponent<Irenderer>();
			go[i]->GetComponentManager()->AddComponent<Renderer3D>();
			go[i]->GetComponentManager()->GetComponent<Transform>()->Translate(xoffset, 0, -15);
			xoffset += 10;
		}
		static_cast<Renderer3D*>(go[0]->GetComponentManager()->GetComponent<Irenderer>())->SetColor(COLORS::Orange);
		static_cast<Renderer3D*>(go[0]->GetComponentManager()->GetComponent<Irenderer>())->LoadMesh(doc.LoadMesh("src/Mesh/Stonefbx.fbx"));
		static_cast<Renderer3D*>(go[1]->GetComponentManager()->GetComponent<Irenderer>())->SetColor(COLORS::Green);
		static_cast<Renderer3D*>(go[1]->GetComponentManager()->GetComponent<Irenderer>())->LoadMesh(doc.LoadMesh("src/Mesh/Tree.fbx"));
		static_cast<Renderer3D*>(go[2]->GetComponentManager()->GetComponent<Irenderer>())->SetColor(COLORS::DarkGray);
		static_cast<Renderer3D*>(go[2]->GetComponentManager()->GetComponent<Irenderer>())->LoadMesh(doc.LoadMesh("src/Mesh/Stonefbx.fbx"));
	}

	virtual void OnGuiRender() {
		Gui::GetInstance()->xAlign = Gui::GetInstance()->XCENTER;
		glm::vec3 pos = Window::GetCamera().GetPosition();
		Gui::GetInstance()->Text("X %f Y %f Z %f",true,0,0,76,COLORS::White,2,pos.x,pos.y,pos.z);
		Gui::GetInstance()->xAlign = Gui::GetInstance()->LEFT;
	}

	virtual void OnUpdate() override {
		if (Input::IsKeyPressed(Keycode::KEY_BACKSPACE)) {
			Window::GetCamera().SetOrthographic(-16.f, 16.f, 9.f, -9.f, 0.1, 100000);
		}
		else if (Input::IsKeyPressed(Keycode::KEY_F)) {
			Window::GetCamera().SetPerspective(1,16,9,0.1,100000);
		}

		for (size_t i = 0; i < 3; i++)
		{
			go[i]->GetComponentManager()->GetComponent<Transform>()->Rotate(-1, glm::vec3(1, 1, 1));
		}
		
	}

	virtual void OnClose() {
	}

};