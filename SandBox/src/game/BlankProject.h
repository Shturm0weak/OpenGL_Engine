#pragma once

class Obj : public GameObject {
public:
	float u_EdgeThickness = 2;
	float u_EdgeSharpness = 30;
	float u_EdgeSubstruct = 0.3;
	float u_GlowStrength = 10;

	Obj() : GameObject() {
		EventSystem::GetInstance()->RegisterClient(EventType::ONUPDATE, this);
		EventSystem::GetInstance()->RegisterClient(EventType::ONSTART, this);
		EventSystem::GetInstance()->RegisterClient(EventType::ONMOVE, this);
		EventSystem::GetInstance()->RegisterClient(EventType::ONROTATE, this);
		EventSystem::GetInstance()->RegisterClient(EventType::ONSCALE, this);
		EventSystem::GetInstance()->RegisterClient(EventType::ONTRANSLATE, this);
		GetComponentManager()->AddComponent<Renderer3D>()->LoadMesh(MeshManager::GetMesh("cube"));
		GetComponent<Renderer3D>()->shader = Shader::Get("Tron");
		Renderer3D* r = GetComponentManager()->GetComponent<Renderer3D>();
		r->floatUniforms.insert(std::make_pair("u_EdgeThickness", &u_EdgeThickness));
		r->floatUniforms.insert(std::make_pair("u_EdgeSharpness", &u_EdgeSharpness));
		r->floatUniforms.insert(std::make_pair("u_EdgeSubstruct", &u_EdgeSubstruct));
		r->floatUniforms.insert(std::make_pair("u_GlowStrength", &u_GlowStrength));
	}

	virtual void OnUpdate() {
		ImGui::Begin("Shader props");
		ImGui::PushID(1000 * GetId());
		ImGui::SliderFloat("u_EdgeThickness",&u_EdgeThickness,0,100);
		ImGui::SliderFloat("u_EdgeSharpness", &u_EdgeSharpness, 0, 100);
		ImGui::SliderFloat("u_EdgeSubstruct", &u_EdgeSubstruct, 0, 100);
		ImGui::SliderFloat("u_GlowStrength", &u_GlowStrength, 0, 100);
		ImGui::PopID();
		ImGui::End();
		/*ThreadPool::Instance()->enqueue([=] {
			std::lock_guard<std::mutex> lock(mtx);
			std::cout << "OnAnotherThread " << std::this_thread::get_id() << std::endl;
			std::function<void()>* f2 = new std::function<void()>([=] {GetComponentManager()->GetComponent<Transform>()->Move(0.1, 0.1, 0.1); std::cout << "OnMainThread " << std::this_thread::get_id() << "\n"; });
			EventSystem::GetInstance()->SendEvent(EventType::ONMAINTHREADPROCESS, nullptr, f2); }
		);*/
	}
};

#include <numeric>
class BlankProject : public Application {
public:
	Obj* go = nullptr;
	Shader* shader = nullptr;
	BlankProject(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_3D, width, height, Vsync) {}
	std::vector<std::string> faces = {
				"src/SkyBox/back.png",
				"src/SkyBox/front.png",
				"src/SkyBox/top.png",
				"src/SkyBox/bottom.png",
				"src/SkyBox/left.png",
				"src/SkyBox/right.png",
	};
	virtual void OnStart() {
		ImGui::SetCurrentContext(Window::imGuiContext);
		shader = Shader::Get("Tron");
		GameObject* dirLight = new GameObject("Directional Light", 0, 0, 0);
		dirLight->GetComponentManager()->AddComponent<DirectionalLight>();
		dirLight->GetComponent<Transform>()->RotateOnce(-50,0,0);
		SkyBox* skybox = new SkyBox(faces, nullptr);
		MeshManager::GetMeshWhenLoaded("cube",(void*)(skybox->GetComponentManager()->GetComponent<Renderer3D>()));
		Gui::GetInstance()->FontBind(Gui::GetInstance()->GetStandartFonts()[Gui::StandartFonts::ARIAL]);
		go = new Obj();
		go->GetComponent<Renderer3D>()->MakeTransparent();
	}

	virtual void OnUpdate() {
	}

	virtual void OnGuiRender() {
		Gui* g = Gui::GetInstance();
		g->xAlign = g->XCENTER;
		g->Text("FPS %f",1,0,0,80,COLORS::White,3,Window::GetFPS());
	}

};