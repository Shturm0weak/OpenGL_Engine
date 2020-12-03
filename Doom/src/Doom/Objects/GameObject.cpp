#include "../pch.h"
#include "../Render/Renderer.h"
#include "GameObject.h"
#include "../Core/World.h"

using namespace Doom;

GameObject::GameObject(const std::string name,float x, float y,float z) {
	this->m_Name = name;
	m_Layer = World::objects.size();
	m_Id = World::m_ObjId;
	World::m_ObjId++;
	m_ComponentManager = new ComponentManager(this, this->m_Name);
	m_Transform = m_ComponentManager->AddComponent<Transform>();
	m_Transform->Translate(x, y, z);
	World::objects.push_back(this);
}

Doom::GameObject::~GameObject()
{
#ifdef _DEBUG
	std::cout << "GameObject: <" << NAMECOLOR << m_Name << RESET << "> has been destroyed\n";
#endif
	EventSystem::GetInstance()->UnregisterAll(this);
	delete m_ComponentManager;
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
	return Utils::GetPosition(m_Transform->m_PosMat4);
}

void Doom::GameObject::RemoveChild(void * child)
{
	for (size_t i = 0; i < m_Childs.size(); i++)
	{
		if (m_Childs[i] == child) {
			m_Childs.erase(m_Childs.begin() + i);
			return;
		}
	}
}
glm::vec3 GameObject::GetScale() {
	return Utils::GetScale(m_Transform->m_ScaleMat4);
}
