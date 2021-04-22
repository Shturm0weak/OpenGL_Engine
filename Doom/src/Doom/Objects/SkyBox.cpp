#include "../pch.h"
#include "SkyBox.h"
#include "ECS/ComponentManager.h"

using namespace Doom;

GameObject* Doom::SkyBox::CreateSkyBox(std::vector<std::string> faces)
{
	s_Faces = faces;
	GameObject* go = GameObject::Create("SkyBox");
	Renderer3D* r3d = go->m_ComponentManager.AddComponent<Renderer3D>();
	r3d->m_IsSkyBox = true;
	size_t m_RendererID = Texture::LoadCubeMap(faces);
	Texture* texture = new Texture();
	texture->m_RendererID = m_RendererID;
	r3d->m_DiffuseTexture = texture;
	r3d->m_Shader = Shader::Get("SkyBox");
	Mesh::GetMeshWhenLoaded("cube", go->GetComponent<Renderer3D>());
	return go;
}
