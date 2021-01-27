#include "../pch.h"
#include "../Render/Renderer.h"
#include "GameObject.h"
#include "../Core/World.h"

using namespace Doom;

GameObject::GameObject(const std::string name,float x, float y,float z) {
	this->m_Name = name;
	m_Layer = World::s_GameObjects.size();
	m_Id = World::s_ObjId;
	World::s_ObjId++;
	m_ComponentManager = new ComponentManager(this);
	m_Transform = m_ComponentManager->AddComponent<Transform>();
	m_Transform->Translate(x, y, z);
	World::s_GameObjects.push_back(this);
}

Doom::GameObject::GameObject(const GameObject& rhs)
{
	Copy(rhs);
}

void Doom::GameObject::operator=(const GameObject& rhs)
{
	Copy(rhs);
}

Doom::GameObject::~GameObject()
{
#ifdef _DEBUG
	std::cout << "GameObject: <" << NAMECOLOR << m_Name << RESET << "> has been destroyed\n";
#endif
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

void Doom::GameObject::Copy(const GameObject& rhs)
{
	m_Name = rhs.m_Name;
	m_Enable = rhs.m_Enable;
	m_IsParticle = rhs.m_IsParticle;
	m_IsSerializable = rhs.m_IsSerializable;
	m_ComponentManager->operator=(*rhs.m_ComponentManager);
	for (uint32_t i = 0; i < rhs.m_Childs.size(); i++)
	{
		GameObject* go = World::CreateGameObject();
		go->operator=(*(GameObject*)rhs.m_Childs[i]);
		AddChild(go);
	}
}

glm::vec3 GameObject::GetScale() {
	return Utils::GetScale(m_Transform->m_ScaleMat4);
}
