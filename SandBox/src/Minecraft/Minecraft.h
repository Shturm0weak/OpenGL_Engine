#pragma once

class Minecraft : public Application {
public:
	Minecraft(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_3D, width, height, Vsync) {}
	std::vector<GameObject*> cube;
	std::vector<GameObject*> tree;
	virtual void OnStart() override {
		Renderer::Light = new GameObject("Light", 0, 20);
		std::vector<std::string> faces = {
				"src/SkyBox/back.png",
				"src/SkyBox/front.png",
				"src/SkyBox/top.png",
				"src/SkyBox/bottom.png",
				"src/SkyBox/left.png",
				"src/SkyBox/right.png",
		};
		MeshManager::LoadMesh("skyboxCube", "src/Mesh/cube.fbx");
		MeshManager::LoadMesh("cube", "src/Minecraft/Models/cube.fbx");
		MeshManager::LoadMesh("tree", "src/Mesh/Tree.fbx");
		SkyBox* skybox = new SkyBox(faces, MeshManager::GetMesh("skyboxCube"));

		for (size_t i = 0; i < 16; i++)
		{
			for (size_t j = 0; j < 16; j++)
			{
				for (size_t k = 0; k < 16; k++)
				{
					cube.push_back(new GameObject("dirt", 0, 0));
					cube.back()->GetComponentManager()->RemoveComponent<Irenderer>();
					cube.back()->GetComponentManager()->AddComponent<Renderer3D>();
					cube.back()->GetComponent<Renderer3D>()->LoadMesh(MeshManager::GetMesh("cube"));
					cube.back()->GetComponent<Renderer3D>()->mat.ambient = 0.4f;
					cube.back()->GetComponent<Renderer3D>()->ChangeRenderTechnic(Renderer3D::RenderTechnic::Instancing);
					cube.back()->GetComponent<Renderer3D>()->diffuseTexture = Texture::Create("src/Minecraft/Textures/minecraft_dirt.png");
					cube.back()->GetComponent<Transform>()->RotateOnce(0, 0, -90);
					cube.back()->GetComponent<Transform>()->Translate(i * 2, k * 2,j * 2);
				}
			}
		}
	}

	virtual void OnUpdate() override {
	}
};