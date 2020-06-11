#include "../pch.h"
#include "GameObject.h"

using namespace Doom;

GameObject::GameObject(const std::string name,float x, float y) {
	Renderer2DLayer::name = name;
	Renderer2DLayer::type = "GameObject";
	id = Renderer2DLayer::obj_id;
	Renderer2DLayer::obj_id++;
	component_manager = new ComponentManager(this, this->name);
	component_manager->AddComponent<Transform>();
	component_manager->GetComponent<Transform>()->position.x = position.x = x;
	component_manager->GetComponent<Transform>()->position.y = position.y = y;
	component_manager->AddComponent<SpriteRenderer>();
	//glGenVertexArrays(1, &this->vao);
	//glBindVertexArray(this->vao);
	//if (rendertype == Render2D) {
	//	this->layout->Push<float>(2);
	//	this->layout->Push<float>(2);
	//}
	//else if (rendertype == Render3D)
	//{
	//	this->layout->Push<float>(2);
	//	this->layout->Push<float>(2);
	//}
	//this->va->AddBuffer(*this->vb, *this->layout);
	//this->vb->UnBind();
	//this->ib->UnBind();
	Renderer2DLayer::objects2d.push_back(this);
}

void GameObject::SetName(const char * _name)
{
	name = _name;
}

Doom::GameObject::~GameObject()
{
	//std::cout << "GameObject destroyed\n";
	EventSystem::Instance()->UnregisterAll(this);
	delete component_manager;
}

void GameObject::operator=(GameObject& go) {
	this->SetName(go.name.c_str());
	this->name.append("(clone)");
	Transform* tr = this->GetComponentManager()->GetComponent<Transform>();
	tr->RotateOnce(go.GetAngle(), glm::vec3(0, 0, 1));
	tr->Translate(go.GetPositions().x, go.GetPositions().y);
	tr->Scale(go.scaleValues[0], go.scaleValues[1]);
	SpriteRenderer* thisRenderer = this->GetComponentManager()->GetComponent<SpriteRenderer>();
	SpriteRenderer* goRenderer = go.GetComponentManager()->GetComponent<SpriteRenderer>();
	thisRenderer->SetColor(glm::vec4(goRenderer->color[0], goRenderer->color[1], goRenderer->color[2], goRenderer->color[3]));
	thisRenderer->SetTexture(goRenderer->texture);
	thisRenderer->SetUVs(goRenderer->GetUVs());
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

GameObject::Position GameObject::GetPositions(){
	 Transform* tr = this->GetComponentManager()->GetComponent<Transform>();
	 position.x = tr->position.x;
	 position.y = tr->position.y;
	 return position;
}

float* GameObject::GetScale() {	return scaleValues;}

float GameObject::GetAngle() { return component_manager->GetComponent<Transform>()->angle; }
