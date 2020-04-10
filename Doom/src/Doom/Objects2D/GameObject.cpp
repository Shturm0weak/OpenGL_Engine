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
	if (go.shadertype == 1)
		this->SetColor(glm::vec4(go.color[0], go.color[1], go.color[2], go.color[3]));
	else if (go.shadertype == 0) {
		this->SetColor(glm::vec4(go.color[0], go.color[1], go.color[2], go.color[3]));
		this->SetTexture(go.GetPathToTexture()->c_str());
	}
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
	delete(texture);
	texture = nullptr;
	*pathToTexture = path;
	texture = new Texture(path);
	if(ShaderType::SHADER_TEXTURE != shadertype)
		SetShader(GameObject::SHADER_TEXTURE);
}

void GameObject::SetTexture(Texture* texture) {
	this->texture = nullptr;
	this->texture = texture;
	if (ShaderType::SHADER_TEXTURE != shadertype)
		SetShader(GameObject::SHADER_TEXTURE);
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

float GameObject::GetAngle() { return component_manager->GetComponent<Transform>()->angle; }
