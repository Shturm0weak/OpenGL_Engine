#include "pch.h"
#include "AimTrainer.h"
#include "Rays/Ray3D.h"
#include "Text/Gui.h"
#include "Render/Instancing.h"
#include "Objects/SkyBox.h"
#include "Core/SceneSerializer.h"
#include "Core/Timer.h"

using namespace Doom;

void AimTrainer::RayCast()
{
	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
	{
		Ray3D::Hit hit;
		glm::dvec3 dir = Window::GetInstance().GetCamera().GetForwardV();
		Ray3D::Normilize(dir);
		std::map<double, CubeCollider3D*> intersects = Ray3D::RayCast(Window::GetInstance().GetCamera().GetPosition(), dir, &hit, 10000, false, m_IgnoreMask);
		//new Line(Window::GetCamera().GetPosition(), Window::GetCamera().GetPosition() + dir * 100.f);
		if (hit.m_Object != nullptr)
		{
			SoundManager::GetInstance().Play(SoundManager::GetInstance().GetSound("Hit"));
			if (hit.m_Object->GetOwnerOfComponent()->m_Tag == "Start")
			{
				SpawnObject();
				SpawnObject();
				SpawnObject();
				SpawnObject();
			}
			else 
			{
				m_Objs.erase(hit.m_Object->GetOwnerOfComponent());
				SpawnObject();
				m_Hits++;
			}
			World::GetInstance().DeleteObject(hit.m_Object->GetOwnerOfComponent()->m_Id);
		}
		else
			m_Missed++;
	}
}

void AimTrainer::CameraMovement()
{
	glm::dvec2 delta = ViewPort::GetInstance().GetMouseDragDelta();
	delta *= 0.1;
	delta *= m_MouseSensetivity;
	glm::vec3 rot = Window::GetInstance().GetCamera().GetRotation();
	Window::GetInstance().GetCamera().SetRotation(glm::vec3(rot.x + glm::radians(delta.y), rot.y - glm::radians(delta.x), 0));
}

void AimTrainer::SpawnObject()
{
	GameObject* go = GameObject::Create("obj");
	Renderer3D* r = go->m_ComponentManager.AddComponent<Renderer3D>();
	r->LoadMesh(Mesh::GetMesh("cube"));
	go->m_Tag = "Target";
	go->m_IsSerializable = false;
	glm::vec3 pos = RandomPos(m_LimX, m_LimY, m_LimZ);
	go->m_Transform.Translate(pos);
	r->m_Shader = Shader::Get("Tron");
	r->m_FloatUniforms.insert(std::make_pair("u_EdgeThickness", 2));
	r->m_FloatUniforms.insert(std::make_pair("u_EdgeSharpness", 30));
	r->m_FloatUniforms.insert(std::make_pair("u_EdgeSubstruct", 0.3));
	r->m_FloatUniforms.insert(std::make_pair("u_GlowStrength", 10));
	m_Objs.insert(std::make_pair(go, 0.3));
}

void AimTrainer::UpdateObjects()
{
	for (auto iter = m_Objs.begin(); iter != m_Objs.end();) 
	{
		iter->second += DeltaTime::GetDeltaTime();
		iter->first->m_Transform.Scale(glm::vec3(sin(iter->second)));

		if (iter->first->m_Transform.GetScale().x < 0.2) 
		{
			World::GetInstance().DeleteObject(iter->first->m_Id);
			m_Objs.erase(iter++);
			SpawnObject();
			m_Missed++;
		}
		else
			iter++;
	}
}

glm::vec3 AimTrainer::RandomPos(glm::vec2 limX, glm::vec2 limY, glm::vec2 limZ)
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
	Window::GetInstance().GetCamera().SetPosition(glm::vec3(0, 12, 12));
	Window::GetInstance().GetCamera().SetRotation(glm::vec3(0, 0, 0));
	GameObject* startGo = GameObject::Create("Start", 0, 10, -10);
	Renderer3D* r = startGo->m_ComponentManager.AddComponent<Renderer3D>();
	startGo->m_IsSerializable = false;
	r->LoadMesh(Mesh::GetMesh("cube"));
	r->m_Color = COLORS::Green;
	r->m_Material.m_Ambient = 5;
	startGo->m_Tag = "Start";
	r->m_Material.m_Ambient = 5000;
	Sound* sound = new Sound("src/AimTrainer/Sounds/HitSound.ogg");
	SoundManager::GetInstance().CreateSoundAsset("Hit", sound);
	m_IgnoreMask.push_back("Target");
	m_IgnoreMask.push_back("Start");
	Utils::LoadTexturesFromFolder("src/AimTrainer/CrossHairs");
	std::vector<Texture*> textures = Texture::GetLoadedTexturesFromFolder("src/AimTrainer/CrossHairs");
	m_CrossHair = textures[0];
}

void AimTrainer::OnUpdate()
{
	m_Time += DeltaTime::s_Deltatime;
	if (Input::IsKeyPressed(Keycode::KEY_ESCAPE)) 
	{
		if (m_Pause == false) 
		{
			Window::GetInstance().ShowCursor();
			m_Pause = true;
		}
		else if (m_Pause == true) 
		{
			Window::GetInstance().DisableCursor();
			m_Pause = false;
			m_Options = false;
			m_ChooseCrossHair = false;
		}
	}
	if (!m_Pause) 
	{
		CameraMovement();
		RayCast();
		UpdateObjects();
	}
}

void AimTrainer::OnClose()
{
	m_Objs.clear();
}

void AimTrainer::MainMenu() 
{
	Gui& g = Gui::GetInstance().GetInstance();
	g.m_XAlign = Gui::AlignHorizontally::XCENTER;
	g.RelateToPanel();
	g.m_RelatedPanelProperties.m_AutoAllignment = true;
	g.m_RelatedPanelProperties.m_Margin.y = 50;
	g.m_RelatedPanelProperties.m_Padding.y = 25;
	g.Panel(L"##1", 0, 0, 800, 600, glm::vec4(0.3, 0.3, 0.3, 0.8));
	g.m_XAlign = g.XCENTER;
	if (g.Button(L"Continue", 0, 0, 50, 300, 150, COLORS::Gray * 0.8f)) {
		m_Pause = false;
		Window::GetInstance().DisableCursor();
	}
	g.m_XAlign = g.XCENTER;
	if (g.Button(L"Options", 0, 0, 50, 300, 150, COLORS::Gray * 0.8f))
		m_Options = true;
	g.m_XAlign = g.XCENTER;
	if (g.Button(L"Exit", 0, 0, 50, 300, 150, COLORS::Gray * 0.8f))
		Window::GetInstance().Exit();
	g.m_RelatedPanelProperties.m_AutoAllignment = false;
	g.UnRelateToPanel();
	g.m_XAlign = g.LEFT;
}

void AimTrainer::OptionsMenu()
{
	Gui& g = Gui::GetInstance();
	g.m_XAlign = Gui::AlignHorizontally::XCENTER;
	g.RelateToPanel();
	g.m_RelatedPanelProperties.m_AutoAllignment = true;
	g.m_RelatedPanelProperties.m_Padding.y = 25.0f;
	g.m_RelatedPanelProperties.m_PanelLabelSize = 40.0f;
	g.m_XAlign = g.XCENTER;
	g.Panel(L"Options", 0, 0, 800, 600, glm::vec4(0.3, 0.3, 0.3, 0.8));
	g.m_XAlign = g.LEFT;
	g.SliderFloat(L"Cross hair scale", &m_CrossHairScale, 0.1f, 5.0f, 0, 0, 200, 50);
	g.SliderFloat(L"Audio volume", &m_AudioVolume, 0.0f, 1.0f, 0, 0, 200, 50);
	g.SliderFloat(L"Mouse sensetivity", &m_MouseSensetivity, 0.1f, 3.0f, 0, 0, 200, 50);
	g.CheckBox(L"Draw shadows", &m_DrawShadows, 0, 0, 25);
	g.CheckBox(L"Bloom", &Renderer::s_BloomEffect, 0, 0, 25);
	if (g.Button(L"Choose crosshair", 0, 0, 30, 400, 50, COLORS::Gray * 0.8f))
		m_ChooseCrossHair = true;
	Instancing::GetInstance()->m_DrawShadows = m_DrawShadows ? 1.0f : 0.0f;
	SoundManager::GetInstance().SetVolume(m_AudioVolume);
	g.m_RelatedPanelProperties.m_AutoAllignment = false;
	if (g.Button(L"Back", 0, -475, 30, 200, 50, COLORS::Gray * 0.8f))
		m_Options = false;
	g.UnRelateToPanel();
	g.m_XAlign = g.LEFT;
}

void AimTrainer::ChooseCrossHairMenu() 
{
	Gui& g = Gui::GetInstance();
	g.m_XAlign = Gui::AlignHorizontally::XCENTER;
	g.RelateToPanel();
	g.Panel(L"##Choose CrossHair", 0, 0, 800, 600, glm::vec4(0.3, 0.3, 0.3, 0.8));
	float tempMarginX = g.m_RelatedPanelProperties.m_Margin.x;
	g.m_RelatedPanelProperties.m_Margin.x = 25;
	g.m_XAlign = g.LEFT;
	std::vector<Texture*> textures = Texture::GetLoadedTexturesFromFolder("src/AimTrainer/CrossHairs");
	glm::vec2 pos{ 0, 0 };
	uint32_t counter = 0;
	for (uint32_t i = 0; i < textures.size(); i++)
	{
		float size = 100;
		pos.x = (size + size * 0.5f) * counter;
		if (pos.x + size * 0.5f + g.m_RelatedPanelProperties.m_Margin.x >= 800) 
		{
			pos.y -= (size + size * 0.5f);
			pos.x = 0;
			counter = 0;
		}
		else counter++;
		if (g.Button(L"", pos.x, pos.y, 30, size, size, COLORS::Gray * 0.8f, COLORS::Gray * 0.5f, COLORS::White, textures[i])) 
		{
			m_ChooseCrossHair = false;
			m_CrossHair = textures[i];
		}
	}
	g.m_RelatedPanelProperties.m_Margin.x = tempMarginX;
	g.UnRelateToPanel();
	g.m_XAlign = g.LEFT;
}

void AimTrainer::OnGuiRender()
{
	Timer t;
	if (!m_Pause) 
	{
		Gui& g = Gui::GetInstance();
		if (m_CrossHair != nullptr)
			g.Image(0, 0, 50 * m_CrossHairScale, 50 * m_CrossHairScale, m_CrossHair);
		g.Text(L"FPS %f", 1, 700, 500, 20, COLORS::White, 3, Window::GetInstance().GetFPS());
		g.Text(L"Missed %d", 1, 700, 450, 20, COLORS::White, 0, (int64_t)m_Missed);
		g.Text(L"Hits %d", 1, 700, 400, 20, COLORS::White, 0, (int64_t)m_Hits);
		g.Text(L"Time %f", 1, 700, 350, 20, COLORS::White, 2, m_Time);
		if (m_Hits > 0)
			g.Text(L"Accuracy %f#%", 1, 700, 300, 20, COLORS::White, 0, ((float)m_Hits / (float)(m_Hits + m_Missed)) * 100.0f);

		g.m_XAlign = g.XCENTER;
		g.Text(L"%d", 1, 0, 600, 40, COLORS::White, 0, (int64_t)m_Hits);
		g.m_XAlign = g.LEFT;
	}
	else 
	{
		if (m_ChooseCrossHair)
			ChooseCrossHairMenu();
		else if (!m_Options)
			MainMenu();
		else
			OptionsMenu();
	}
}