#pragma once


class TestImGui : public Application {
public:
	std::vector<GameObject*> go;
	double t = 0;

	TestImGui(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_3D, width, height, Vsync) {}

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

		MeshManager::LoadMesh("man", "src/Mesh/man.fbx");
		MeshManager::LoadMesh("cube", "src/Mesh/box1.fbx");
		MeshManager::LoadMesh("sphere", "src/Mesh/sphere.fbx");

		Renderer::Light = new GameObject("Light",0,20);
		Renderer::Light->GetComponentManager()->GetComponent<Transform>()->Translate(0, 5, 0);
		Renderer::Light->GetComponentManager()->GetComponent<Transform>()->Scale(0.1, 0.1, 0.1);
		Renderer::Light->GetComponentManager()->RemoveComponent<Irenderer>();
		Renderer::Light->GetComponentManager()->AddComponent<Renderer3D>();
		static_cast<Renderer3D*>(Renderer::Light->GetComponentManager()->GetComponent<Irenderer>())->SetColor(COLORS::White);
	    static_cast<Renderer3D*>(Renderer::Light->GetComponentManager()->GetComponent<Irenderer>())->LoadMesh(MeshManager::GetMesh("cube"));
		static_cast<Renderer3D*>(go[0]->GetComponentManager()->GetComponent<Irenderer>())->SetColor(COLORS::White);
		static_cast<Renderer3D*>(go[0]->GetComponentManager()->GetComponent<Irenderer>())->LoadMesh(MeshManager::GetMesh("man"));
		go[0]->GetComponentManager()->GetComponent<Transform>()->Scale(0.1, 0.1, 0.1); 
		static_cast<Renderer3D*>(go[0]->GetComponentManager()->GetComponent<Irenderer>())->diffuseTexture = new Texture("src/Images/man.png");
		static_cast<Renderer3D*>(go[0]->GetComponentManager()->GetComponent<Irenderer>())->normalMapTexture = new Texture("src/Images/normalMap.png");
		static_cast<Renderer3D*>(go[1]->GetComponentManager()->GetComponent<Irenderer>())->diffuseTexture = new Texture("src/Images/Box1.png");
		static_cast<Renderer3D*>(go[1]->GetComponentManager()->GetComponent<Irenderer>())->normalMapTexture = new Texture("src/Images/Box1normalmap.png");
		static_cast<Renderer3D*>(go[1]->GetComponentManager()->GetComponent<Irenderer>())->SetColor(COLORS::White);
		static_cast<Renderer3D*>(go[1]->GetComponentManager()->GetComponent<Irenderer>())->LoadMesh(MeshManager::GetMesh("cube"));
	}

	virtual void OnGuiRender() {
	}

	virtual void OnUpdate() override {
		go[0]->GetComponentManager()->GetComponent<Transform>()->Translate(sin(t) * 15, 0, sin(t) * 15);
		go[0]->GetComponentManager()->GetComponent<Transform>()->Rotate(30,30,30);
		go[1]->GetComponentManager()->GetComponent<Transform>()->Rotate(30, 30, 30);
		t += DeltaTime::deltatime;

		for (size_t i = 0; i < 1; i++)
		{
			//go[i]->GetComponentManager()->GetComponent<Transform>()->Rotate(1,1,1);
		}
		
	}

	virtual void OnClose() {
	}

};