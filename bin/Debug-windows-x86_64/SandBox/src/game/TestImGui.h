#pragma once

#include "Render/SkyBox.h"
#include "Core/Ray3D.h"

class TestImGui : public Application {
public:
	std::vector<GameObject*> go;
	double t = 0;
	TestImGui(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_3D, width, height, Vsync) {}
	Ray3D::Hit hit;
	GameObject* light = nullptr;
	virtual void OnStart() override {
		ImGui::SetCurrentContext(Window::imGuiContext);
		float xoffset = -10.f;
		std::vector<std::string> faces = {
				"src/SkyBox/back.png",
				"src/SkyBox/front.png",
				"src/SkyBox/top.png",
				"src/SkyBox/bottom.png",
				"src/SkyBox/left.png",
				"src/SkyBox/right.png",
		};
		light = new GameObject("dirLight");
		light->GetComponentManager()->AddComponent<DirectionalLight>();
		MeshManager::LoadMesh("skyboxCube", "src/Mesh/cube.fbx");
		SkyBox* skybox = new SkyBox(faces,MeshManager::GetMesh("skyboxCube"));
		for (size_t i = 0; i < 1; i++)
		{
			go.push_back(new GameObject());
			go[i]->GetComponentManager()->RemoveComponent<Irenderer>();
			go[i]->GetComponentManager()->AddComponent<Renderer3D>();
			go[i]->GetComponentManager()->GetComponent<Transform>()->Translate(xoffset, 0, 0);
			xoffset += 10;
		}

		
		MeshManager::LoadMesh("cube1", "src/Mesh/box1.fbx");
		MeshManager::LoadMesh("sphere", "src/Mesh/sphere.fbx");
		MeshManager::LoadMesh("man", "src/Mesh/man.fbx");

		for (int i = 0; i < 1; i++)
		{
			static_cast<Renderer3D*>(go[i]->GetComponentManager()->GetComponent<Irenderer>())->SetColor(COLORS::White);
			//MeshManager::GetMeshWhenLoaded("man", static_cast<Renderer3D*>(go[i]->GetComponentManager()->GetComponent<Irenderer>()));
			go[i]->GetComponentManager()->GetComponent<Transform>()->Scale(0.1, 0.1, 0.1);
			static_cast<Renderer3D*>(go[i]->GetComponentManager()->GetComponent<Irenderer>())->diffuseTexture = Texture::Create("src/Images/man.png");
			static_cast<Renderer3D*>(go[i]->GetComponentManager()->GetComponent<Irenderer>())->normalMapTexture = Texture::Create("src/Images/normalMap.png");
			static_cast<Renderer3D*>(go[i]->GetComponentManager()->GetComponent<Irenderer>())->ChangeRenderTechnic(Renderer3D::RenderTechnic::Forward);
			static_cast<Renderer3D*>(go[i]->GetComponentManager()->GetComponent<Irenderer>())->mesh = (MeshManager::GetMesh("man"));
		}
		//static_cast<Renderer3D*>(go[1]->GetComponentManager()->GetComponent<Irenderer>())->diffuseTexture = new Texture("src/Images/Box1.png");
		//static_cast<Renderer3D*>(go[1]->GetComponentManager()->GetComponent<Irenderer>())->normalMapTexture = new Texture("src/Images/Box1normalmap.png");
		//static_cast<Renderer3D*>(go[1]->GetComponentManager()->GetComponent<Irenderer>())->SetColor(COLORS::White);
		//static_cast<Renderer3D*>(go[1]->GetComponentManager()->GetComponent<Irenderer>())->LoadMesh(MeshManager::GetMesh("skyboxCube"));
		//go[1]->GetComponentManager()->AddComponent<CubeCollider3D>();
		//go[0]->GetComponentManager()->AddComponent<CubeCollider3D>();
	}

	virtual void OnGuiRender() {
	}

	virtual void OnUpdate() override {
		if (Input::IsKeyPressed(Keycode::KEY_G)) {
			//new Line(Window::GetCamera().GetPosition(),Window::GetCamera().GetPosition() + Window::GetCamera().GetMouseDirVec() * 100.0f);
			//MeshManager::AsyncLoadMesh("man", "src/Mesh/man.fbx");
		}
		glm::vec3 forward = glm::vec3(-Window::GetCamera().forwardV.x, Window::GetCamera().forwardV.y,-Window::GetCamera().forwardV.z);
		Ray3D::RayCast(Window::GetCamera().GetPosition(), forward,&hit,100);
		//go[0]->GetComponentManager()->GetComponent<Transform>()->Translate(sin(t) * 15, 0, cos(t) * 15);
		//go[0]->GetComponentManager()->GetComponent<Transform>()->Rotate(30, 30, 30);
		//go[1]->GetComponentManager()->GetComponent<Transform>()->Rotate(30, 30, 30);
		//t += DeltaTime::deltatime;

		for (size_t i = 0; i < 1; i++)
		{
			//go[i]->GetComponentManager()->GetComponent<Transform>()->Rotate(1,1,1);
		}
		
	}

	virtual void OnClose() {
	}

};