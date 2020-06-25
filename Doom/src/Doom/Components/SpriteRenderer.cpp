
#include "../pch.h"
#include "SpriteRenderer.h"
#include "../Render/Batch.h"

using namespace Doom;

void SpriteRenderer::SetTexture(const std::string& path) {
	texture = nullptr;
	pathToTexture = path;
	texture = new Texture(path);
}

void SpriteRenderer::SetTexture(Texture* texture) {
	if (texture == nullptr) {
		pathToTexture = "None";
		this->texture = nullptr;
		this->texture = texture;
		return;
	}
	this->texture = nullptr;
	this->texture = texture;
	pathToTexture = texture->GetFilePath();
}

void SpriteRenderer::InitShader() {
	shader = Shader::defaultShader;
}

Doom::SpriteRenderer::SpriteRenderer(GameObject* _owner)
{
	SetType("Renderer");
	this->owner = _owner;
	tr = owner->GetComponentManager()->GetComponent<Transform>();
	this->pos = translate(glm::mat4(1.f), glm::vec3(tr->position.x, tr->position.y, 0));
	InitShader();
}

Doom::SpriteRenderer::~SpriteRenderer()
{
}

void Doom::SpriteRenderer::Update(glm::vec3 pos)
{
	this->pos = translate(glm::mat4(1.f), pos);
	float WorldVerPos[16];
	glm::mat4 scaleXview = view * scale;
	float* pSource;
	pSource = (float*)glm::value_ptr(scaleXview);
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			WorldVerPos[i * 4 + j] = 0;
			for (unsigned int k = 0; k < 4; k++) {
				WorldVerPos[i * 4 + j] += mesh2D[i * 4 + k] * pSource[k * 4 + j];
			}
		}
	}
	WorldVertexPositions[0] = WorldVerPos[0];
	WorldVertexPositions[1] = WorldVerPos[1];
	WorldVertexPositions[2] = WorldVerPos[4];
	WorldVertexPositions[3] = WorldVerPos[5];
	WorldVertexPositions[4] = WorldVerPos[8];
	WorldVertexPositions[5] = WorldVerPos[9];
	WorldVertexPositions[6] = WorldVerPos[12];
	WorldVertexPositions[7] = WorldVerPos[13];
	pSource = nullptr;
}

void Doom::SpriteRenderer::Render()
{
	Batch::GetInstance()->Submit(*this);
}

std::string SpriteRenderer::GetPathToTexture() { return this->pathToTexture; }

void SpriteRenderer::ReverseUVs()
{
	float v1 = mesh2D[2];
	float v2 = mesh2D[3];
	float v3 = mesh2D[6];
	float v4 = mesh2D[7];
	float v5 = mesh2D[10];
	float v6 = mesh2D[11];
	float v7 = mesh2D[14];
	float v8 = mesh2D[15];
	mesh2D[2] = v3;
	mesh2D[3] = v4;
	mesh2D[6] = v1;
	mesh2D[7] = v2;
	mesh2D[10] = v7;
	mesh2D[11] = v8;
	mesh2D[14] = v5;
	mesh2D[15] = v6;
}

void SpriteRenderer::ReversedUvs()
{
	mesh2D[2] = 1.f;
	mesh2D[3] = 0.f;
	mesh2D[6] = 0.f;
	mesh2D[7] = 0.f;
	mesh2D[10] = 0.f;
	mesh2D[11] = 1.f;
	mesh2D[14] = 1.f;
	mesh2D[15] = 1.f;
}

void SpriteRenderer::OriginalUvs()
{
	mesh2D[2] = 0.f;
	mesh2D[3] = 0.f;
	mesh2D[6] = 1.f;
	mesh2D[7] = 0.f;
	mesh2D[10] = 1.f;
	mesh2D[11] = 1.f;
	mesh2D[14] = 0.f;
	mesh2D[15] = 1.f;
}

void SpriteRenderer::SetUVs(float* uvs)
{
	mesh2D[2] = uvs[0];
	mesh2D[3] = uvs[1];
	mesh2D[6] = uvs[2];
	mesh2D[7] = uvs[3];
	mesh2D[10] = uvs[4];
	mesh2D[11] = uvs[5];
	mesh2D[14] = uvs[6];
	mesh2D[15] = uvs[7];
}

double Doom::SpriteRenderer::GetWidth()
{
	return owner->scaleValues[0] * mesh2D[4] * 2;
}

double Doom::SpriteRenderer::GetHeight()
{
	return owner->scaleValues[1] * mesh2D[9] * 2;
}

float * SpriteRenderer::GetUVs()
{
	float uvs[8];
	uvs[0] = mesh2D[2];
	uvs[1] = mesh2D[3];
	uvs[2] = mesh2D[6];
	uvs[3] = mesh2D[7];
	uvs[4] = mesh2D[10];
	uvs[5] = mesh2D[11];
	uvs[6] = mesh2D[14];
	uvs[7] = mesh2D[15];
	return uvs;
}

void SpriteRenderer::Setlayer(int layer)
{
	unsigned int size = Renderer::objects2d.size();
	std::cout << owner->name << " is set from layer " << owner->GetId() << " to " << layer << std::endl;
	Renderer::objects2d.erase(Renderer::objects2d.begin() + owner->GetId());
	Renderer::objects2d.insert(Renderer::objects2d.begin() + layer, owner);
	for (unsigned int i = 0; i < Renderer::objects2d.size(); i++)
	{
		Renderer::objects2d[i]->SetId(i);
		Renderer::objects2d[i]->GetLayer() = i;
	}
	return;
}

void Doom::Renderer3D::LoadMesh(Mesh * mesh)
{
	this->mesh = mesh;
	delete layout;
	delete va;
	delete vb;
	delete ib;
	layout = new VertexBufferLayout();
	vb = new VertexBuffer(this->mesh->mesh, this->mesh->meshSize * sizeof(double));
	va = new VertexArray();
	ib = new IndexBuffer(this->mesh->indicesForNormals, this->mesh->indicesSize);
	layout->Push<double>(3);
	layout->Push<double>(3);
	layout->Push<double>(2);
	layout->Push<double>(3);
	layout->Push<double>(3);
	va->AddBuffer(*this->vb, *this->layout);
	va->UnBind();
	shader->UnBind();
	vb->UnBind();
	ib->UnBind();
}

Doom::Renderer3D::Renderer3D(GameObject* _owner)
{
	renderType = "3D";
	SetType("Renderer");
	owner = _owner;
	shader = new Shader("src/Shaders/Basic3D.shader");
	tr = owner->GetComponentManager()->GetComponent<Transform>();
	pos = translate(glm::mat4(1.f), glm::vec3(tr->position.x, tr->position.y, tr->position.z));
}

Doom::Renderer3D::~Renderer3D()
{
	delete layout;
	delete va;
	delete vb;
	delete ib;
	delete mesh;
}

void Doom::Renderer3D::Render()
{
	if (mesh != nullptr) {
		this->shader->Bind();
		glBindTextureUnit(0, this->diffuseTexture->m_RendererID);
		this->pos = translate(glm::mat4(1.f), glm::vec3(tr->position.x, tr->position.y, tr->position.z));
		Window::GetCamera().RecalculateViewMatrix();
		this->shader->UploadUnifromMat4("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
		this->shader->SetUniformMat4f("u_Model", pos);
		this->shader->SetUniformMat4f("u_View", view);
		this->shader->SetUniformMat4f("u_Scale", scale);
		this->shader->SetUniform4fv("m_color", color);
		this->shader->SetUniform3fv("u_LightPos", glm::vec3(Renderer::Light->GetPositions().x, Renderer::Light->GetPositions().y, Renderer::Light->GetPositions().z));
		this->shader->SetUniform3fv("u_LightColor", glm::vec3(Renderer::Light->GetComponentManager()->GetComponent<Irenderer>()->color));
		this->shader->SetUniform3fv("u_CameraPos",Window::GetCamera().GetPosition());
		this->shader->SetUniform1f("u_Ambient", mat.ambient); 
		this->shader->SetUniform1f("u_Specular", mat.specular);
		this->shader->SetUniform1i("u_DiffuseTexture", 0);
		if (useNormalMap) {
			glBindTextureUnit(1, this->normalMapTexture->m_RendererID);
			this->shader->SetUniform1i("u_NormalMapTexture", 1);
		}
		this->shader->SetUniform1i("u_isNormalMapping", useNormalMap);
		this->shader->Bind();
		va->Bind();
		ib->Bind();
		vb->Bind();
		Renderer::Vertices += mesh->meshSize / 2;
		Renderer::DrawCalls++;
		glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
		shader->UnBind();
		ib->UnBind();
	}
}
