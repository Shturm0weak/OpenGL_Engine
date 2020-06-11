#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "../Render/Renderer2D.h"
#include "Component.h"

#include "../Components/Collision.h"
#include "../Components/Transform.h"
#include "../Components/Animator.h"
#include "../Components/SpriteRenderer.h"

namespace Doom {

	class DOOM_API ComponentManager {
		GameObject* owner = nullptr;
		std::string owner_name;
		const char** items = nullptr;
		std::vector <Component*> components;

	public:
		~ComponentManager() { 
			delete[] items;
			RemoveComponent<Collision>();
			RemoveComponent<Transform>();
			RemoveComponent<Animator>();
		}
		ComponentManager(GameObject* owner, std::string& owner_name) {
			this->owner = owner;
			this->owner_name = owner_name;
		}

		int GetAmountOfComponents() { return components.size(); }

		const char** GetItems() {
			if (items != nullptr)
				delete[] items;
			items = new const char*[components.size()];
			for (unsigned int i = 0; i < components.size(); i++)
			{
				items[i] = components[i]->GetComponentType();
			}
			return items;
		}

		template <class T >
		void RemoveComponent() {
			return;
		}

		template <>
		void RemoveComponent<Collision>() {
			Collision* col = nullptr;
			col = GetComponent<Collision>();
			if (col == nullptr)
				return;
			int _id = col->GetId();
			if (col->m_Id < components.size())
				components.erase(components.begin() + col->m_Id);
			unsigned int _size = components.size();
			if (col->m_Id != _size) {
				for (unsigned int i = col->m_Id; i < _size; i++)
				{
					components[i]->m_Id = i;;
				}
			}
			Renderer2DLayer::collision2d.erase(_id + Renderer2DLayer::collision2d.begin());
			Renderer2DLayer::col_id--;
			unsigned int size = Renderer2DLayer::collision2d.size();
			if (_id != size) {
				for (unsigned int i = _id; i < size; i++)
				{
					Renderer2DLayer::collision2d[i]->SetId(i);
				}
			}
			GetComponent<Transform>()->col = nullptr;
			delete col;
			return;
		}

		template <>
		void RemoveComponent<Transform>() {
			Transform* col = nullptr;
			col = GetComponent<Transform>();
			if (col == nullptr)
				return;
			components.erase(components.begin() + col->m_Id);
			unsigned int _size = components.size();
			if (col->m_Id != _size) {
				for (unsigned int i = col->m_Id; i < _size; i++)
				{
					components[i]->m_Id = i;;
				}
			}
			col->col = nullptr;
			delete col;
			return;
		}

		template <>
		void RemoveComponent<Animator>() {
			Animator* col = nullptr;
			col = GetComponent<Animator>();
			if (col == nullptr)
				return;
			components.erase(components.begin() + col->m_Id);
			unsigned int _size = components.size();
			if (col->m_Id != _size) {
				for (unsigned int i = col->m_Id; i < _size; i++)
				{
					components[i]->m_Id = i;;
				}
			}
			delete col;
			return;
		}

		template <>
		void RemoveComponent<SpriteRenderer>() {
			SpriteRenderer* col = nullptr;
			col = GetComponent<SpriteRenderer>();
			if (col == nullptr)
				return;
			components.erase(components.begin() + col->m_Id);
			unsigned int _size = components.size();
			if (col->m_Id != _size) {
				for (unsigned int i = col->m_Id; i < _size; i++)
				{
					components[i]->m_Id = i;;
				}
			}
			delete col;
			return;
		}

		template <class T >
		T* GetComponent() {
			return nullptr;
		}

		template<>
		Collision* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				if (components[i]->GetComponentType() == "Collision") {
					return (Collision*)components[i];
				}
			}
			//std::cout << yellow << "Warning:" << white << " there is no component of type <Collision> for gameobject: " << owner_name << std::endl;
			return nullptr;
		}

		template<>
		SpriteRenderer* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				if (components[i]->GetComponentType() == "SpriteRenderer") {
					return (SpriteRenderer*)components[i];
				}
			}
			//std::cout << yellow << "Warning:" << white << " there is no component of type <Collision> for gameobject: " << owner_name << std::endl;
			return nullptr;
		}

		template<>
		Animator* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				if (components[i]->GetComponentType() == "Animator") {
					return (Animator*)components[i];
				}
			}
			//std::cout << yellow << "Warning:" << white << " there is no component of type <Collision> for gameobject: " << owner_name << std::endl;
			return nullptr;
		}



		template<>
		Transform* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				Component* comp = components[i];
				if (comp->GetComponentType() == "Transform") {
					return (Transform*)comp;
				}
			}
			//std::cout << yellow << "Warning:" << white << " there is no component of this type <Transform> for gameobject: " << owner_name << std::endl;
			return nullptr;
		}

		template<class T>
		T* AddComponent() {

		}

		template <>
		Collision* AddComponent<Collision>() {
			if (GetComponent<Collision>() == nullptr) {
				Collision* object = new Collision(owner);
				object->SetOwner(this->owner);
				object->m_Id = components.size();
				components.push_back(object);
				return object;
			}
			return nullptr;
		}

		template <>
		SpriteRenderer* AddComponent<SpriteRenderer>() {
			if (GetComponent<SpriteRenderer>() == nullptr) {
				SpriteRenderer* object = new SpriteRenderer(owner);
				object->owner = (this->owner);
				object->m_Id = components.size();
				components.push_back(object);
				return object;
			}
			return nullptr;
		}

		template <>
		Animator* AddComponent<Animator>() {
			if (GetComponent<Animator>() == nullptr) {
				Animator* animator = new Animator();
				animator->owner = owner;
				animator->m_Id = components.size();
				components.push_back(animator);
				return animator;
			}
			return nullptr;
		}

		template <>
		Transform* AddComponent<Transform>() {
			if (GetComponent<Transform>() == nullptr) {
				Transform* object = new Transform();
				object->owner = owner;
				object->init();
				object->m_Id = components.size();
				components.push_back(object);
				return object;
			}
			return nullptr;
		}
	};
}
#endif // !1