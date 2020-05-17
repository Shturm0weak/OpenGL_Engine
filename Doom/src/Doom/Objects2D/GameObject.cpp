#include "../pch.h"
#include "GameObject.h"

using namespace Doom;

GameObject::GameObject(const std::string name,float x, float y) {
	*Renderer2DLayer::name = name;
	*Renderer2DLayer::type = "GameObject";
	id = Renderer2DLayer::obj_id;
	Renderer2DLayer::obj_id++;
	component_manager = new ComponentManager(this, *this->name);
	component_manager->AddComponent<Transform>();
	component_manager->GetComponent<Transform>()->position.x = position.x = x;
	component_manager->GetComponent<Transform>()->position.y = position.y = y;
	this->pos = translate(glm::mat4(1.f), glm::vec3(position.x, position.y, 0));
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);
	if (rendertype == Render2D) {
		this->layout->Push<float>(2);
		this->layout->Push<float>(2);
	}
	else if (rendertype == Render3D)
	{
		this->layout->Push<float>(2);
		this->layout->Push<float>(2);
	}
	this->va->AddBuffer(*this->vb, *this->layout);
	InitShader();
	this->va->UnBind();
	this->shader->UnBind();
	this->vb->UnBind();
	this->ib->UnBind();
	Renderer2DLayer::PushObj(*this);
}

void GameObject::SetName(const char * _name)
{
	delete name;
	name = new std::string(_name);
}

void GameObject::operator=(GameObject go) {
	this->SetName(go.name->c_str());
	this->name->append("(clone)");
	Transform* tr = this->GetComponentManager()->GetComponent<Transform>();
	tr->RotateOnce(go.GetAngle(), glm::vec3(0, 0, 1));
	tr->Translate(go.GetPositions().x, go.GetPositions().y);
	tr->Scale(go.scaleValues[0], go.scaleValues[1]);
	this->SetRenderType((RenderType)go.GetRenderType());
	this->SetColor(glm::vec4(go.color[0], go.color[1], go.color[2], go.color[3]));
	this->SetTexture(go.texture);
	this->SetUVs(go.GetUVs());
	if (go.GetCollisionReference() != nullptr) {
		Collision* col = this->GetComponentManager()->AddComponent<Collision>();
		Collision* gocol = (Collision*)go.GetCollisionReference();
		col->SetOffset(gocol->offsetX, gocol->offsetY);
		col->Enable = gocol->Enable;
		col->Translate(position.x, position.y);
		col->IsTrigger = gocol->IsTrigger;
		col->SetTag(gocol->GetTag());
	}
}

void GameObject::OnRunning(OrthographicCamera& camera) {
		this->shader->Bind();
		//this->pos = translate(glm::mat4(1.f), glm::vec3(position.x, position.y, 0));
		this->viewXscale = scale * view;
		if (Static == false) {
			this->MVP = camera.GetProjectionMatrix() * pos * viewXscale;
			this->shader->UploadUnifromMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		}
		else {
			this->MVP = ViewProjecTionRelatedToScreen * pos * viewXscale;
			this->shader->UploadUnifromMat4("u_ViewProjection", ViewProjecTionRelatedToScreen);
		}
		this->shader->SetUniformMat4f("u_MVP", this->MVP);
		if (shadertype == SHADER_TEXTURE)
			this->texture->Bind(this->texture->m_RendererID);
	Renderer2DLayer::Draw(*this->va, *this->ib, *this->shader);
}


void GameObject::SetTexture(const std::string& path) {
	texture = nullptr;
	*pathToTexture = path;
	texture = new Texture(path);
}

void GameObject::SetTexture(Texture* texture) {
	this->texture = nullptr;
	this->texture = texture;
	*pathToTexture = texture->GetFilePath();
}

void GameObject::InitShader() {
	if (texture == (void*)0xCCCCCCCC && shadertype == SHADER_TEXTURE || texture == nullptr && shadertype == SHADER_TEXTURE) {
		std::cout  << "InitShader" << " : "  << "Texture doesn't exist!"  << std::endl;
		shadertype = SHADER_COLOR;
	}
	if (shader != (void*)0xCCCCCCCC || shader != nullptr) {
		shader->UnBind();
		delete(shader);
		shader = nullptr;
	}
	if (shadertype == SHADER_TEXTURE) {
		shader = new Shader("src/Shaders/Basic.shader");
		shader->Bind();
		this->texture->Bind(this->texture->m_RendererID);
		this->shader->SetUniform1i("u_Texture", this->texture->m_RendererID);
	}
	else if (shadertype == SHADER_COLOR) {
		shader = new Shader("src/Shaders/newshader.shader");
		shader->Bind();
		shader->SetUniform4fv("U_Color",this->color);
	}
	else if (shadertype == SHADER_BLOOM) {
		shader = new Shader("src/Shaders/Bloom.shader");
		shader->Bind();
		if (texture == nullptr)
			delete texture;
		texture = new Texture("src/Images/bird.png");
		this->texture->Bind(this->texture->m_RendererID);
		this->shader->SetUniform1i("u_Texture", this->texture->m_RendererID);
	}

}

void GameObject::SetShader(int _enum) {
	if (_enum == 1) {
		shadertype = SHADER_COLOR;
		if (this->texture != (void*)0xCCCCCCCC && this->texture != nullptr && component_manager->GetComponent<Animator>() == nullptr) {
			delete texture;
			texture = nullptr;
		}
		InitShader();
	}
	else if (_enum == 0) {
		shadertype = SHADER_TEXTURE;
		InitShader();
	}
	else if (_enum == 2){
		shadertype = SHADER_BLOOM;
		InitShader();
	}
}

void GameObject::SetRenderType(RenderType type)
{
	rendertype = type;
	delete va;
	delete vb;
	delete ib;
	layout->Clear();
	va = new VertexArray();
	if (rendertype == Render2D) {
		ib = new IndexBuffer(indeces2D,6);
		vb = new VertexBuffer(mesh2D, 4 * 4 * sizeof(float));
		this->layout->Push<float>(2);
		this->layout->Push<float>(2);
	}
	else if (rendertype == Render3D)
	{
		ib = new IndexBuffer(indeces3D, 36);
		vb = new VertexBuffer(mesh3D, 24 * sizeof(float));
		this->layout->Push<float>(3);
	}
	this->va->AddBuffer(*this->vb, *this->layout);
}

void GameObject::SetColor(vec4 color) {
	if (this->color == color)
		return;
	this->color = color;
}

GameObject::Position GameObject::GetPositions(){
	 Transform* tr = this->GetComponentManager()->GetComponent<Transform>();
	 position.x = tr->position.x;
	 position.y = tr->position.y;
	 return position;
}

std::string* GameObject::GetPathToTexture() { return this->pathToTexture; }

float* GameObject::GetColor() { 
	float* color = new float[4];
	for (unsigned int i = 0; i < 4; i++)
	{
		color[i] = this->color[i];
	}
	return color;
}

int GameObject::GetRenderType() { return rendertype; }

int GameObject::GetShaderType() { return shadertype; }

float* GameObject::GetScale() {	return scaleValues;}

void Doom::GameObject::ReverseUVs()
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

void Doom::GameObject::ReversedUvs()
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

void Doom::GameObject::OriginalUvs()
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

void Doom::GameObject::SetUVs(float* uvs)
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

float * Doom::GameObject::GetUVs()
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

float GameObject::GetAngle() { return component_manager->GetComponent<Transform>()->angle; }
