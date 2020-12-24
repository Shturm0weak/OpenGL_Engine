#include "AimTrainer.h"
#include <iostream>
#include "Core/ViewPort.h"
#include "Rays/Ray3D.h"
#include "Objects/Line.h"
#include "Text/Gui.h"
#include <random>
#include "Audio/SoundManager.h"
#include "Render/Instancing.h"
#include "Objects/SkyBox.h"
#include "Core/Utils.h"
#include "Core/SceneSerializer.h"

using namespace Doom;

void Doom::AimTrainer::RayCast()
{
	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1)) {
		Ray3D::Hit hit;
		glm::vec3 forward = Window::GetCamera().forwardV;
		glm::vec3 dir = glm::vec3(-forward.x, forward.y, -forward.z);
		Ray3D::Normilize(dir);
		std::map<float, CubeCollider3D*> intersects = Ray3D::RayCast(Window::GetCamera().GetPosition(), dir, &hit, 10000, false, m_IgnoreMask);
		//new Line(Window::GetCamera().GetPosition(), Window::GetCamera().GetPosition() + dir * 100.f);
		if (hit.m_Object != nullptr) {
			SoundManager::Play(SoundManager::GetSound("Hit"));
			if (hit.m_Object->GetOwnerOfComponent()->m_Tag == "Start") {
				SpawnObject();
				SpawnObject();
				SpawnObject();
				SpawnObject();
			}
			else {
				m_Objs.erase(hit.m_Object->GetOwnerOfComponent());
				SpawnObject();
				m_Hits++;
			}
			World::DeleteObject(hit.m_Object->GetOwnerOfComponent()->m_Id);
		}
		else {
			m_Missed++;
		}
	}
}

void Doom::AimTrainer::CameraMovement()
{
	glm::dvec2 delta = ViewPort::GetInstance()->GetMouseDragDelta();
	delta *= 0.1;
	delta *= m_MouseSensetivity;
	glm::vec3 rot = Window::GetCamera().GetRotation();
	Window::GetCamera().SetRotation(glm::vec3((rot.x + delta.y * (2 * 3.14159f) / 360.0f), (rot.y - delta.x * (2 * 3.14159f) / 360.0f), 0));
}

void Doom::AimTrainer::SpawnObject()
{
	GameObject* go = new GameObject("obj");
	Renderer3D* r = go->GetComponentManager()->AddComponent<Renderer3D>();
	r->LoadMesh(MeshManager::GetMesh("cube"));
	go->m_Tag = "Target";
	go->m_IsSerializable = false;
	glm::vec3 pos = RandomPos(m_LimX, m_LimY, m_LimZ);
	go->m_Transform->Translate(pos);
	r->m_Shader = Shader::Get("Tron");
	r->m_FloatUniforms.insert(std::make_pair("u_EdgeThickness", 2));
	r->m_FloatUniforms.insert(std::make_pair("u_EdgeSharpness", 30));
	r->m_FloatUniforms.insert(std::make_pair("u_EdgeSubstruct", 0.3));
	r->m_FloatUniforms.insert(std::make_pair("u_GlowStrength", 10));
	m_Objs.insert(std::make_pair(go, 0.3));
}

void Doom::AimTrainer::UpdateObjects()
{
	for (auto iter = m_Objs.begin(); iter != m_Objs.end();) {
		iter->second += DeltaTime::GetDeltaTime();
		iter->first->m_Transform->Scale(glm::vec3(sin(iter->second)));

		if (iter->first->m_Transform->GetScale().x < 0.2) {
			World::DeleteObject(iter->first->m_Id);
			m_Objs.erase(iter++);
			SpawnObject();
			m_Missed++;
		}
		else {
			iter++;
		}
	}
}

glm::vec3 Doom::AimTrainer::RandomPos(glm::vec2 limX, glm::vec2 limY, glm::vec2 limZ)
{
	glm::vec3 pos;
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_real_distribution<float> distribution(limX.x, limX.y);
	pos.x = distribution(e2);
	std::uniform_real_distribution<float> distribution1(limY.x, limY.y);
	pos.y = distribution1(e2);
	std::uniform_real_distribution<float> distribution2(limZ.x, limZ.y);
	pos.z = distribution2(e2);
	return pos;
}

void AimTrainer::OnStart()
{
	SceneSerializer::DeSerialize(SceneSerializer::s_CurrentSceneFilePath);
	Window::GetCamera().SetPosition(glm::vec3(0, 12, 12));
	Window::GetCamera().SetRotation(glm::vec3(0, 0, 0));
	GameObject* startGo = new GameObject("Start", 0, 10, -10);
	Renderer3D* r = startGo->GetComponentManager()->AddComponent<Renderer3D>();
	startGo->m_IsSerializable = false;
	r->LoadMesh(MeshManager::GetMesh("cube"));
	r->m_Color = COLORS::Green;
	r->m_Material.m_Ambient = 5;
	startGo->m_Tag = "Start";
	r->m_Material.m_Ambient = 5000;
	Sound* sound = new Sound("src/AimTrainer/Sounds/HitSound.ogg");
	SoundManager::CreateSoundAsset("Hit", sound);
	m_IgnoreMask.push_back("Target");
	m_IgnoreMask.push_back("Start");
	Utils::LoadTexturesFromFolder("src/AimTrainer/CrossHairs");
	std::vector<Texture*> textures = Texture::GetLoadedTexturesFromFolder("src/AimTrainer/CrossHairs");
	m_CrossHair = textures[0];
}

void AimTrainer::OnUpdate()
{
	m_Time += DeltaTime::s_Deltatime;
	if (Input::IsKeyPressed(Keycode::KEY_ESCAPE)) {
		if (m_Pause == false) {
			Window::ShowCursor();
			m_Pause = true;
		}
		else if (m_Pause == true) {
			Window::DisableCursor();
			m_Pause = false;
			m_Options = false;
			m_ChooseCrossHair = false;
		}
	}
	if (!m_Pause) {
		CameraMovement();
		RayCast();
		UpdateObjects();
	}
}

void AimTrainer::OnClose()
{
	m_Objs.clear();
}

void Doom::AimTrainer::MainMenu() {
	Gui* g = Gui::GetInstance();
	g->m_XAlign = Gui::AlignHorizontally::XCENTER;
	g->RelateToPanel();
	g->m_RelatedPanelProperties.m_AutoAllignment = true;
	g->m_RelatedPanelProperties.m_Margin.y = 50;
	g->m_RelatedPanelProperties.m_Padding.y = 25;
	g->Panel("##1", 0, 0, 800, 600, glm::vec4(0.3, 0.3, 0.3, 0.8));
	g->m_XAlign = g->XCENTER;
	if (g->Button("Continue", 0, 0, 50, 300, 150, COLORS::Gray * 0.8f)) {
		m_Pause = false;
		Window::DisableCursor();
	}
	g->m_XAlign = g->XCENTER;
	if (g->Button("Options", 0, 0, 50, 300, 150, COLORS::Gray * 0.8f)) {
		m_Options = true;
	}
	g->m_XAlign = g->XCENTER;
	if (g->Button("Exit", 0, 0, 50, 300,150, COLORS::Gray * 0.8f)) {
		Window::Exit();
	}
	g->m_RelatedPanelProperties.m_AutoAllignment = false;
	g->UnRelateToPanel();
	g->m_XAlign = g->LEFT;
}

void Doom::AimTrainer::OptionsMenu() {
	Gui* g = Gui::GetInstance();
	g->m_XAlign = Gui::AlignHorizontally::XCENTER;
	g->RelateToPanel();
	g->m_RelatedPanelProperties.m_AutoAllignment = true;
	g->m_RelatedPanelProperties.m_Padding.y = 25.0f;
	g->m_RelatedPanelProperties.m_PanelLabelSize = 40.0f;
	g->m_XAlign = g->XCENTER;
	g->Panel("Options", 0, 0, 800, 600, glm::vec4(0.3, 0.3, 0.3, 0.8));
	g->m_XAlign = g->LEFT;
	g->SliderFloat("Cross hair scale", &m_CrossHairScale, 0.1f, 5.0f, 0, 0, 200, 50);
	g->SliderFloat("Audio volume", &m_AudioVolume, 0.0f, 1.0f, 0, 0, 200, 50);
	g->SliderFloat("Mouse sensetivity", &m_MouseSensetivity, 0.1f, 3.0f, 0, 0, 200, 50);
	g->CheckBox("Draw shadows", &m_DrawShadows, 0, 0, 25);
	g->CheckBox("Bloom", &Renderer::s_BloomEffect, 0, 0, 25);
	if (g->Button("Choose crosshair", 0, 0, 30, 400, 50, COLORS::Gray * 0.8f)) {
		m_ChooseCrossHair = true;
	}
	Instancing::Instance()->m_DrawShadows = m_DrawShadows ? 1.0f : 0.0f;
	SoundManager::SetVolume(m_AudioVolume);
	g->m_RelatedPanelProperties.m_AutoAllignment = false;
	if (g->Button("Back", 0, -475, 30, 200, 50, COLORS::Gray * 0.8f)) {
		m_Options = false;
	}
	g->UnRelateToPanel();
	g->m_XAlign = g->LEFT;
}

void Doom::AimTrainer::ChooseCrossHairMenu() {
	Gui* g = Gui::GetInstance();
	g->m_XAlign = Gui::AlignHorizontally::XCENTER;
	g->RelateToPanel();
	g->Panel("##Choose CrossHair", 0, 0, 800, 600, glm::vec4(0.3, 0.3, 0.3, 0.8));
	float tempMarginX = g->m_RelatedPanelProperties.m_Margin.x;
	g->m_RelatedPanelProperties.m_Margin.x = 25;
	g->m_XAlign = g->LEFT;
	std::vector<Texture*> textures = Texture::GetLoadedTexturesFromFolder("src/AimTrainer/CrossHairs");
	glm::vec2 pos{ 0, 0 };
	uint32_t counter = 0;
	for (uint32_t i = 0; i < textures.size(); i++)
	{
		float size = 100;
		pos.x = (size + size * 0.5f) * counter;
		if (pos.x + size * 0.5f + g->m_RelatedPanelProperties.m_Margin.x >= 800) {
			pos.y -= (size + size * 0.5f);
			pos.x = 0;
			counter = 0;
		}
		else
			counter++;
		if (g->Button("", pos.x, pos.y, 30, size, size, COLORS::Gray * 0.8f, COLORS::Gray * 0.5f, COLORS::White, textures[i])) {
			m_ChooseCrossHair = false;
			m_CrossHair = textures[i];
		}
	}
	g->m_RelatedPanelProperties.m_Margin.x = tempMarginX;
	g->UnRelateToPanel();
	g->m_XAlign = g->LEFT;
}

void Doom::AimTrainer::OnGuiRender()
{
	if (!m_Pause) {
		Gui* g = Gui::GetInstance();
		if(m_CrossHair != nullptr)
			g->Image(0, 0, 50 * m_CrossHairScale, 50 * m_CrossHairScale, m_CrossHair);
		g->Text("FPS %f", 1, 700, 500, 40, COLORS::White, 3, Window::GetFPS());
		g->Text("Missed %d", 1, 700, 450, 40, COLORS::White, 0, m_Missed);
		g->Text("Hits %d", 1, 700, 400, 40, COLORS::White, 0, m_Hits);
		g->Text("Time %f", 1, 700, 350, 40, COLORS::White, 2, m_Time);
		if (m_Hits > 0)
			g->Text("Accuracy %f#%", 1, 700, 300, 40, COLORS::White, 0, ((float)m_Hits / (float)(m_Hits + m_Missed)) * 100.0f);

		g->m_XAlign = g->XCENTER;
		g->Text("%d", 1, 0, 600, 80, COLORS::White, 0, m_Hits);
		g->m_XAlign = g->LEFT;
	}
	else {
		if (m_ChooseCrossHair)
			ChooseCrossHairMenu();
		else if (!m_Options)
			MainMenu();
		else
			OptionsMenu();
	}
}