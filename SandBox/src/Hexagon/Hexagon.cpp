#include "Hexagon.h"

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

glm::vec4 Doom::Hexagon::Colors(float value)
{
	glm::vec4 color = COLORS::White;
	if (value < 0.3)
		color = COLORS::Blue;
	else if (value < 0.5)
		color = COLORS::Yellow;
	else if (value < 0.8)
		color = COLORS::Green;
	else
		color = COLORS::Gray;
	return color;
}


void Doom::Hexagon::OnStart()
{
	float* seed = new float[width * height];
	float* noise = new float[width * height];

	for (size_t i = 0; i < width * height; i++)
	{
		seed[i] = (float)rand() / (float)RAND_MAX;
	}
	PerlinNoise2D(width, height, seed, 5, 0.7, noise);
	/*for (size_t i = 0; i < width; i++)
	{
		for (size_t j = 0; j < height; j++)
		{
			std::cout << noise[i * (size_t)width + j] << " ";
		}
		std::cout << std::endl;
	}*/
	delete[] seed;

	Mesh* hex = MeshManager::GetInstance().GetMesh("hex");
	float hexX = hex->m_TheHighestPoint.x * 2;
	float hexZ = hex->m_TheHighestPoint.z * 2;
	for (uint32_t i = 0; i < width; i++)
	{
		for (uint32_t j = 0; j < height; j++)
		{
			GameObject* go = new GameObject("hex");
			if (i % 2 == 1)
				go->m_Transform->Translate(j * hexX + hexX * 0.5f, 0, i * 1.5f);
			else
				go->m_Transform->Translate(j * hexX, 0, i * 1.5f);
			Renderer3D* r = go->AddComponent<Renderer3D>();
			r->LoadMesh(MeshManager::GetInstance().GetMesh("hex"));
			r->ChangeRenderTechnic(Renderer3D::RenderTechnic::Instancing);
			r->m_Color = Colors(noise[i * (int)width + j]);
			go->m_Transform->RotateOnce(glm::vec3(-90,0,0));
		}
	}
}

void Doom::Hexagon::OnUpdate()
{
}

void Doom::Hexagon::OnClose()
{
}

void Doom::Hexagon::OnGuiRender()
{
}
