#pragma once

void PerlinNoise2D(int nWidth, int nHeight, float *fSeed, int nOctaves, float fBias, float *fOutput)
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

class Minecraft : public Application {
public:
	Minecraft(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_3D, width, height, Vsync) {}
	std::vector<GameObject*> cube;
	std::vector<GameObject*> tree;
	GameObject* terrain = nullptr;
	GameObject* cup = nullptr;
	std::vector<GameObject*> lights;
	int width = 128;
	int height = 128;
	virtual void OnStart() override {

		float* seed = new float[width * height];
		float* noise = new float[width * height];
		terrain = new GameObject("terrain");
		
		for (size_t i = 0; i < width * height; i++)
		{
			seed[i] = (float)rand() / (float)RAND_MAX;
		}
		PerlinNoise2D(width, height,seed,5,0.7,noise);
		delete[] seed;
		std::vector<std::string> faces = {
				"src/SkyBox/back.png",
				"src/SkyBox/front.png",
				"src/SkyBox/top.png",
				"src/SkyBox/bottom.png",
				"src/SkyBox/left.png",
				"src/SkyBox/right.png",
		};
		MeshManager::LoadMesh("skyboxCube", "src/Mesh/cube.fbx");
		MeshManager::AsyncLoadMesh("cube", "src/Minecraft/Models/cube.fbx");
		SkyBox* skybox = new SkyBox(faces, MeshManager::GetMesh("skyboxCube"));
		for (size_t i = 0; i < width; i++)
		{
			for (size_t j = 0; j < height; j++)
			{
				cube.push_back(new GameObject("dirt"));
				terrain->AddChild((void*)cube.back());
				cube.back()->SetOwner((void*)terrain);
				cube.back()->GetComponentManager()->RemoveComponent<Irenderer>();
				cube.back()->GetComponentManager()->AddComponent<Renderer3D>();
				MeshManager::GetMeshWhenLoaded("cube",(void*)cube.back()->GetComponent<Renderer3D>());
				cube.back()->GetComponent<Renderer3D>()->mat.ambient = 0.4f;
				cube.back()->GetComponent<Renderer3D>()->ChangeRenderTechnic(Renderer3D::RenderTechnic::Instancing);
				cube.back()->GetComponent<Renderer3D>()->diffuseTexture = Texture::Create("src/Minecraft/Textures/minecraft_dirt.png");
				cube.back()->GetComponent<Transform>()->RotateOnce(0, 0, -90);
				cube.back()->GetComponent<Transform>()->Scale(0.5, 0.5, 0.5);
				cube.back()->GetComponent<Transform>()->Translate(i,int(noise[i * height + j] * 15),j);
			}
		}
		for (uint32_t i = 0; i < 7; i++)
		{
			lights.push_back(new GameObject(std::string("light " + std::to_string(i)), i * 10, 10, i * 10));
			lights.back()->GetComponentManager()->AddComponent<PointLight>();
			lights.back()->GetComponentManager()->GetComponent<PointLight>()->color = glm::vec3(i * 0.1, 0, i * 0.1);
		}
		lights.back()->GetComponentManager()->RemoveComponent<PointLight>();
		lights.back()->GetComponentManager()->AddComponent<DirectionalLight>();
	}

	virtual void OnUpdate() override {
	}

	virtual void OnGuiRender() {
		Gui::GetInstance()->xAlign = Gui::AlignHorizontally::XCENTER;
		Gui::GetInstance()->Text("FPS: %f", true, 0, 0, 40,COLORS::White,0,Window::GetFPS());
	}
};