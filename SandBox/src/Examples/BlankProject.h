#pragma once

#include <numeric>
class BlankProject : public Application {
public:
	Shader* shader = nullptr;
	double time = 0;
	GameObject* earth;
	GameObject* moon;
	Line* l = nullptr;
	glm::vec3 start { 1.0f };
	glm::vec3 end { 0,0,10 };
	BlankProject(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name, width, height, Vsync) {}
	std::vector<std::string> faces = {
				"src/SkyBox/skyBox1/1back.png",
				"src/SkyBox/skyBox1/2front.png",
				"src/SkyBox/skyBox1/3top.png",
				"src/SkyBox/skyBox1/4bottom.png",
				"src/SkyBox/skyBox1/5left.png",
				"src/SkyBox/skyBox1/6right.png",
	};
	virtual void OnStart() {
		ImGui::SetCurrentContext(Window::GetInstance().s_ImGuiContext);
		//l = new Line(start, end);
		//shader = Shader::Get("Tron");
		//GameObject* dirLight = new GameObject("Directional Light", 0, 0, 0);
		//dirLight->GetComponentManager()->AddComponent<DirectionalLight>();
		//dirLight->GetComponent<Transform>()->RotateOnce(-50,0,0);
		//SkyBox* skybox = new SkyBox(faces, nullptr);
		//MeshManager::GetMeshWhenLoaded("cube",(void*)(skybox->GetComponentManager()->GetComponent<Renderer3D>()));
		//Gui::GetInstance()->FontBind(Gui::GetInstance()->GetStandartFonts()[Gui::StandartFonts::ARIAL]);
		//earth = new GameObject("Earth", 0, 0, 0);
		//earth->transform->Scale(18, 18, 18);
		//moon = new GameObject("Moon", 0, 6, 0);
		//earth->transform->RotateOnce(70, 0, 0);
		//moon->transform->RotateOnce(70, 0, 0);
		//moon->transform->Scale(5, 5, 5);
		//moon->GetComponentManager()->AddComponent<PointLight>();
		//MeshManager::GetMeshWhenLoaded("sphere", earth->GetComponentManager()->AddComponent<Renderer3D>());
		//MeshManager::GetMeshWhenLoaded("sphere", moon->GetComponentManager()->AddComponent<Renderer3D>());
		//earth = new GameObject("earth", start.x , start.y , start.z);
		//moon = new GameObject("moon");
		//for (int i = -5; i < 4; i++)
		//{
		//	for (int j = -5; j < 4; j++)
		//	{
		//		go = new Obj();
		//		go->transform->Translate(i * 2, 0, j * 2);
		//		go->name = std::to_string(i) + " " + std::to_string(j);
		//	}
		//}
		//Texture::GetAsync(earth, [=] {
		//	Texture* t = Texture::Get("src/Images/earth.png");
		//	if (t != nullptr)
		//		earth->GetComponentManager()->GetComponent<Renderer3D>()->diffuseTexture = t;
		//	return t;
		//});
	}

	virtual void OnUpdate() {
		//glm::vec3 dir{ 1.0f };
		//Transform* tr = earth->GetComponent<Transform>();
		//glm::vec3 _end = tr->m_ViewMat4 * glm::vec4(end, 1.0f);
		//l->SetEndPoint(tr->GetPosition());
		//l->SetEndPoint(_end);
		//std::cout << _end.x << " " << _end.y << " " << _end.z << " " << std::endl;
		//moon->GetComponent<Transform>()->Translate(_end);
		//_end = moon->m_Transform->GetPosition();
		//std::cout << _end.x << " " << _end.y << " " << _end.z << " " << std::endl;
		//std::cout << "end\n";
		//moon->GetComponent<Transform>()->RotateOnce(earth->m_Transform->GetRotation(), true);
		//earth->transform->Rotate( 0, 0, 8);
		//moon->transform->Rotate(0, 0, -30);
		//moon->transform->Translate(glm::sin(time) * 30, 6, glm::cos(time) * 30);
		//time += DeltaTime::deltatime;
	}

	virtual void OnGuiRender() {
		//Gui* g = Gui::GetInstance();
		//g->m_XAlign = g->XCENTER;
		//g->Text("FPS %f",1,0,0, 40,COLORS::White,3,Window::GetFPS());
	}

};