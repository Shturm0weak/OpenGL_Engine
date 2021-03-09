#include "../pch.h"
#include "../Render/Renderer.h"
#include "GameObject.h"
#include "../Core/World.h"

using namespace Doom;

GameObject::GameObject(const std::string name,float x, float y,float z) 
{
	this->m_Name = name;
	m_Transform = m_ComponentManager.AddComponent<Transform>();
}

void Doom::GameObject::Delete()
{
	if (m_Owner != nullptr)
	{
		((GameObject*)m_Owner)->RemoveChild(this);
	}
	m_Childs.clear();
	m_ComponentManager.Clear();
	s_FreeMemory.push_back(m_MemoryPoolPtr);
	std::vector<GameObject*>& goV = World::GetInstance().s_GameObjects;
	auto iter = std::find(goV.begin(), goV.end(), this);
	if (iter != goV.end())
		goV.erase(iter);
#ifdef _DEBUG
	std::cout << "GameObject: <" << NAMECOLOR << m_Name << RESET << "> has been destroyed\n";
#endif
}

Doom::GameObject::GameObject(GameObject& rhs)
{
	Copy(rhs);
}

void Doom::GameObject::operator=(GameObject& rhs)
{
	Copy(rhs);
}

GameObject* Doom::GameObject::Create(const std::string name, float x, float y, float z)
{
	char* ptr = Utils::PreAllocateMemory<GameObject>(s_MemoryPool, s_FreeMemory);
	GameObject* go = (GameObject*)((void*)ptr);// = new(iter->first + iter->second * sizeof(CubeCollider3D)) CubeCollider3D();
	go->m_Transform = go->m_ComponentManager.AddComponent<Transform>();
	go->m_MemoryPoolPtr = ptr;
	go->m_Name = name;
	go->m_Layer = World::GetInstance().s_GameObjects.size();
	go->m_Id = World::GetInstance().s_ObjId;
	World::GetInstance().s_ObjId++;
	go->m_Transform->Translate(x, y, z);
	World::GetInstance().s_GameObjects.push_back(go);
	return go;
}

Doom::GameObject::~GameObject()
{
//#ifdef _DEBUG
//	std::cout << "GameObject: <" << NAMECOLOR << m_Name << RESET << "> has been destroyed\n";
//#endif
	//delete m_ComponentManager;
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
		if (m_Childs[i] == child)
		{
			((GameObject*)m_Childs[i])->m_Owner = nullptr;
			m_Childs.erase(m_Childs.begin() + i);
			return;
		}
	}
}

void Doom::GameObject::Copy(GameObject& rhs)
{
	m_Name = rhs.m_Name;
	m_Enable = rhs.m_Enable;
	m_IsParticle = rhs.m_IsParticle;
	m_IsSerializable = rhs.m_IsSerializable;
	m_ComponentManager.operator=(rhs.m_ComponentManager);
	m_IsSerializalbeChilds = rhs.m_IsSerializalbeChilds;
	if (m_IsSerializalbeChilds == false) return;
	for (uint32_t i = 0; i < rhs.m_Childs.size(); i++)
	{
		GameObject* go = GameObject::Create();
		go->operator=(*(GameObject*)rhs.m_Childs[i]);
		AddChild(go);
	}
}

glm::vec3 GameObject::GetScale() {
	return Utils::GetScale(m_Transform->m_ScaleMat4);
}