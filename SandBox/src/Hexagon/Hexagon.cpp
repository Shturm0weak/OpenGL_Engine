#include "pch.h"
#include "Hexagon.h"
#include "Rays/Ray3D.h"
#include "Core/Timer.h"

void Doom::Hexagon::PerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput)
{
	// Used 1D Perlin Noise
	for (int x = 0; x < nWidth; x++)
		for (int y = 0; y < nHeight; y++)
		{
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < nOctaves; o++)
			{
				int nPitch = nWidth >> o;
				int nSampleX1 = (x / nPitch) * nPitch;
				int nSampleY1 = (y / nPitch) * nPitch;

				int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
				int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

				float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
				float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

				float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
				float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

				fScaleAcc += fScale;
				fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
				fScale = fScale / fBias;
			}

			// Scale to seed range
			fOutput[y * nWidth + x] = fNoise / fScaleAcc;
		}

}

glm::vec4 Doom::Hexagon::Colors(float value, GameObject* sprite)
{
	glm::vec4 color = COLORS::White;
	if (value < 0.3)
		color = COLORS::Blue;
	else if (value < 0.5)
		color = COLORS::Yellow;
	else if (value < 0.8) 
	{
		color = COLORS::Green;
		//sprite->m_Enable = true;
		//sprite->AddComponent<SpriteRenderer>()->m_Texture = Texture::Get("src/Images/corn.png");
	}
	else
		color = COLORS::Gray;
	return color;
}


void Doom::Hexagon::CameraMovement()
{
	float camSpeed = cameraSpeed;
	Camera& camera = Window::GetInstance().GetCamera();
	if (Input::IsKeyDown(Keycode::KEY_LEFT_SHIFT))
		camSpeed *= 2;
	if (Input::IsKeyDown(Keycode::KEY_W))
		camera.SetPosition(camera.GetPosition() + (glm::vec3(  0,  0, -camSpeed) * DeltaTime::GetDeltaTime()));
	if (Input::IsKeyDown(Keycode::KEY_S))
		camera.SetPosition(camera.GetPosition() + (glm::vec3(  0,  0,  camSpeed) * DeltaTime::GetDeltaTime()));
	if (Input::IsKeyDown(Keycode::KEY_D))
		camera.SetPosition(camera.GetPosition() + (glm::vec3(  camSpeed,  0,  0) * DeltaTime::GetDeltaTime()));
	if (Input::IsKeyDown(Keycode::KEY_A))
		camera.SetPosition(camera.GetPosition() + (glm::vec3( -camSpeed,  0,  0) * DeltaTime::GetDeltaTime()));
	if (Input::IsKeyDown(Keycode::SPACE))
		camera.SetPosition(camera.GetPosition() + (glm::vec3(  0,  camSpeed,  0) * DeltaTime::GetDeltaTime()));
	if (Input::IsKeyDown(Keycode::KEY_C))
		camera.SetPosition(camera.GetPosition() + (glm::vec3(  0, -camSpeed,  0) * DeltaTime::GetDeltaTime()));
	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1)) {
		{
			Timer t;
			GameObject* go = World::GetInstance().SelectObject3D();
			if (go != nullptr)
			{
				if (go != selectedObj)
				{
					if (selectedObj != nullptr) {
						Renderer3D* r = selectedObj->GetComponent<Renderer3D>();
						r->m_Color = prevColor;
						selectedObj->m_IsStatic = false;
						std::function<void()>* f = new std::function<void()>([=] { selectedObj->m_IsStatic = true; });
						EventSystem::GetInstance().SendEvent(EventType::ONMAINTHREADPROCESS, nullptr, f);
					}
					selectedObj = go;
					Renderer3D* r = selectedObj->GetComponent<Renderer3D>();
					prevColor = r->m_Color;
					r->m_Color = COLORS::Orange;
					selectedObj->m_IsStatic = false;
					std::function<void()>* f = new std::function<void()>([=] { selectedObj->m_IsStatic = true; });
					EventSystem::GetInstance().SendEvent(EventType::ONMAINTHREADPROCESS, nullptr, f);
				}
			}
		}
		std::cout << Timer::s_OutTime << std::endl;
	}
}

void Doom::Hexagon::OnStart()
{
	GameObject* sun = GameObject::Create("Sun");
	sun->AddComponent<DirectionalLight>();
	sun->m_Transform.RotateOnce(glm::vec3(45, 0, 0));
	Window::GetInstance().GetCamera().SetPosition(glm::vec3(5, 5, 5));
	Window::GetInstance().GetCamera().SetRotation(glm::vec3(-glm::one_over_root_two<float>(), 0, 0));
	float* seed = new float[width * height];
	float* noise = new float[width * height];

	for (size_t i = 0; i < width * height; i++)
	{
		seed[i] = (float)rand() / (float)RAND_MAX;
	}
	PerlinNoise2D(width, height, seed, 5, 0.7, noise);
	delete[] seed;
	Texture::Create("src/Images/corn.png");
	Mesh* hex = Mesh::Get("hex");
	float hexX = hex->m_TheHighestPoint.x * 2;
	float hexZ = hex->m_TheHighestPoint.z * 2;
	for (uint32_t i = 0; i < width; i++)
	{
		for (uint32_t j = 0; j < height; j++)
		{
			GameObject* go = GameObject::Create("Hex");
			//GameObject* sprite = GameObject::Create("Sprite");
			//go->AddChild(sprite);
			//sprite->m_Enable = false;
			go->m_IsStatic = true;
			if (i % 2 == 1)
			{
				go->m_Transform.Translate(j * hexX + hexX * 0.5f, 0, i * 1.5f);
				//sprite->m_Transform.Translate(j * hexX + hexX * 0.5f, 0.15, i * 1.5f);
			}
			else
			{
				go->m_Transform.Translate(j * hexX, 0, i * 1.5f);
				//sprite->m_Transform.Translate(j * hexX, 0.15, i * 1.5f);
			}
			Renderer3D* r = go->AddComponent<Renderer3D>();
			r->LoadMesh(hex);
			r->ChangeRenderTechnic(Renderer3D::RenderTechnic::Instancing);
			r->m_Color = Colors(noise[i * (int)width + j], nullptr);
			go->m_Transform.RotateOnce(glm::vec3(-90, 0, 0));
			go->m_Transform.Scale(glm::vec3(0.99, 0.99, 0.99));
			//sprite->m_Transform.Scale(glm::vec3(0.7, 0.7, 1));
			//sprite->m_Transform.RotateOnce(-90, 0, 0);
		}
	}
}

void Doom::Hexagon::OnUpdate()
{
	CameraMovement();
}

void Doom::Hexagon::OnClose()
{
}

void Doom::Hexagon::OnGuiRender()
{
}