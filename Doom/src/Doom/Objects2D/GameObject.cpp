#include "../pch.h"
#include "../Render/Renderer.h"
#include "GameObject.h"

using namespace Doom;

GameObject::GameObject(const std::string name,float x, float y) {
	this->name = name;
	this->type = "GameObject";
	layer = Renderer::objects2d.size();
	id = Renderer::obj_id;
	Renderer::obj_id++;
	component_manager = new ComponentManager(this, this->name);
	component_manager->AddComponent<Transform>();
	component_manager->GetComponent<Transform>()->position.x = position.x = x;
	component_manager->GetComponent<Transform>()->position.y = position.y = y;
	component_manager->AddComponent<SpriteRenderer>();
	Renderer::objects2d.push_back(this);
}

void GameObject::SetName(const char * _name)
{
	name = _name;
}

Doom::GameObject::~GameObject()
{
	//std::cout << "GameObject destroyed\n";
	EventSystem::GetInstance()->UnregisterAll(this);
	delete component_manager;
}

void GameObject::operator=(GameObject& go) {
	this->SetName(go.name.c_str());
	this->name.append("(clone)");
	Transform* tr = this->GetComponentManager()->GetComponent<Transform>();
	Transform* trgo = go.GetComponentManager()->GetComponent<Transform>();
	tr->RotateOnce(trgo->rotation.x, trgo->rotation.y, trgo->rotation.z,true);
	tr->Translate(go.GetPositions().x, go.GetPositions().y);
	tr->Scale(go.scaleValues[0], go.scaleValues[1]);
	SpriteRenderer* thisRenderer = this->GetComponentManager()->GetComponent<SpriteRenderer>();
	SpriteRenderer* goRenderer = go.GetComponentManager()->GetComponent<SpriteRenderer>();
	thisRenderer->SetColor(glm::vec4(goRenderer->color[0], goRenderer->color[1], goRenderer->color[2], goRenderer->color[3]));
	thisRenderer->SetTexture(goRenderer->texture);
	thisRenderer->SetUVs(goRenderer->GetUVs());
	if (go.GetComponentManager()->GetComponent<Collision>() != nullptr) {
		Collision* col = this->GetComponentManager()->AddComponent<Collision>();
		Collision* gocol = (Collision*)go.GetComponentManager()->GetComponent<Collision>();
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

void Doom::GameObject::RemoveChild(void * child)
{
	for (size_t i = 0; i < Childs.size(); i++)
	{
		if (Childs[i] == child) {
			Childs.erase(Childs.begin() + i);
			return;
		}
	}
}
float* GameObject::GetScale() {	return scaleValues;}

float GameObject::GetAngle() { return component_manager->GetComponent<Transform>()->angleRad; }
