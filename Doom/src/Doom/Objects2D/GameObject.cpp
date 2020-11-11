#include "../pch.h"
#include "../Render/Renderer.h"
#include "GameObject.h"
#include "../Core/World.h"

using namespace Doom;

GameObject::GameObject(const std::string name,float x, float y,float z) {
	this->name = name;
	layer = World::objects.size();
	id = Renderer::obj_id;
	Renderer::obj_id++;
	component_manager = new ComponentManager(this, this->name);
	transform = component_manager->AddComponent<Transform>();
	transform->Translate(x, y, z);
	World::objects.push_back(this);
}

Doom::GameObject::~GameObject()
{
#ifdef _DEBUG
	std::cout << "GameObject: <" << NAMECOLOR << name << RESET << "> has been destroyed\n";
#endif
	EventSystem::GetInstance()->UnregisterAll(this);
	delete component_manager;
}

//void GameObject::operator=(GameObject& go) {
//	this->SetName(go.name.c_str());
//	this->name.append("(clone)");
//	Transform* tr = this->GetComponentManager()->GetComponent<Transform>();
//	Transform* trgo = go.GetComponentManager()->GetComponent<Transform>();
//	tr->RotateOnce(trgo->rotation.x, trgo->rotation.y, trgo->rotation.z,true);
//	tr->Translate(go.GetPositions().x, go.GetPositions().y);
//	tr->Scale(go.scaleValues[0], go.scaleValues[1]);
//	if (go.GetComponentManager()->GetComponent<RectangleCollider2D>() != nullptr) {
//		RectangleCollider2D* col = this->GetComponentManager()->AddComponent<RectangleCollider2D>();
//		RectangleCollider2D* gocol = (RectangleCollider2D*)go.GetComponentManager()->GetComponent<RectangleCollider2D>();
//		col->SetOffset(gocol->offset.x, gocol->offset.y);
//		col->Enable = gocol->Enable;
//		col->Translate(position.x, position.y);
//		col->IsTrigger = gocol->IsTrigger;
//		col->SetTag(gocol->GetTag());
//	}
//}

#include "../Core/Utils.h"

glm::vec3 GameObject::GetPosition() {
	return Utils::GetPosition(transform->pos);
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
glm::vec3 GameObject::GetScale() {
	return Utils::GetScale(transform->scale);
}
