#include "../pch.h"
#include "SkyBox.h"

using namespace Doom;

SkyBox::SkyBox(std::vector<std::string> faces,Mesh* mesh) {
	this->faces = faces;
	m_Name = "SkyBox";
	Renderer3D* r = GetComponentManager()->AddComponent<Renderer3D>();
	r->m_IsSkyBox = true;
	r->m_Mesh = mesh;
	m_RendererID = Texture::LoadCubeMap(faces);
	Texture* texture = new Texture();
	texture->m_RendererID = m_RendererID;
	GetComponent<Renderer3D>()->m_DiffuseTexture = texture;
	GetComponent<Renderer3D>()->m_Shader = Shader::Get("SkyBox");
}