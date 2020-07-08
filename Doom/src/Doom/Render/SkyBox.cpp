#include "../pch.h"
#include "SkyBox.h"

using namespace Doom;

SkyBox::SkyBox(std::vector<std::string> faces, Mesh* mesh) {
	name = "SkyBox";
	GetComponentManager()->RemoveComponent<Irenderer>();
	GetComponentManager()->AddComponent<Renderer3D>()->LoadMesh(mesh);
	m_RendererID = Texture::LoadCubeMap(faces);
	Texture* texture = new Texture();
	texture->m_RendererID = m_RendererID;
	GetComponent<Renderer3D>()->diffuseTexture = texture;
	GetComponent<Renderer3D>()->shader = new Shader("src/Shaders/SkyBox.shader");
}