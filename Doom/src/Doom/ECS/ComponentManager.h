#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "../Render/Renderer2D.h"
#include "Component.h"

#include "../Components/Collision.h"
#include "../Components/Transform.h"
#include "../Components/Animator.h"

namespace Doom {

	class DOOM_API ComponentManager {
		GameObject* owner = nullptr;
		std::string owner_name;
		int maxComponents = 32;
		int currentLength = 0;
		const char** items = nullptr;
		std::vector <std::reference_wrapper<Component>> components;
		void pushcomponents(Component& obj) { components.push_back(obj); }

	public:
		~ComponentManager() { delete[] items; }
		ComponentManager(GameObject* owner, std::string& owner_name) {
			this->owner = owner;
			this->owner_name = owner_name;
		}

		int GetAmountOfComponents() { return currentLength; }

		const char** GetItems() {
			if (items != nullptr)
				delete[] items;
			items = new const char*[components.size()];
			for (unsigned int i = 0; i < components.size(); i++)
			{
				items[i] = components[i].get().GetComponentType();
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
			int _id = col->GetId();
			components.erase(components.begin() + col->m_Id);
			currentLength--;
			unsigned int _size = components.size();
			if (col->m_Id != _size) {
				for (unsigned int i = col->m_Id; i < _size; i++)
				{
					components[i].get().m_Id = i;;
				}
			}
			//std::unique_ptr<Renderer2DLayer> my_p_col(new Renderer2DLayer(std::move(*col)));
			Renderer2DLayer::collision2d.erase(_id + Renderer2DLayer::collision2d.begin());
			Renderer2DLayer::col_id--;
			unsigned int size = Renderer2DLayer::collision2d.size();
			if (_id != size) {
				for (unsigned int i = _id; i < size; i++)
				{
					Renderer2DLayer::collision2d[i].get().SetId(i);
				}
			}
			GetComponent<Transform>()->col = nullptr;
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
				if (components[i].get().GetComponentType() == "Collision") {
					return (Collision*)components[i].get().GetReference();
				}
			}
			//std::cout << yellow << "Warning:" << white << " there is no component of type <Collision> for gameobject: " << owner_name << std::endl;
			return nullptr;
		}

		template<>
		Animator* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				if (components[i].get().GetComponentType() == "Animator") {
					return (Animator*)components[i].get().GetReference();
				}
			}
			//std::cout << yellow << "Warning:" << white << " there is no component of type <Collision> for gameobject: " << owner_name << std::endl;
			return nullptr;
		}



		template<>
		Transform* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				if (components[i].get().GetComponentType() == "Transform") {
					return (Transform*)components[i].get().GetReference();
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
			if (currentLength >= maxComponents) {
				return nullptr;
			}
			else if (GetComponent<Collision>() == nullptr) {
				Collision* object = new Collision(owner);
				object->SetOwner(this->owner);
				object->m_Id = currentLength;
				currentLength++;
				pushcomponents(*object);
				return object;
			}
			return nullptr;
		}

		template <>
		Animator* AddComponent<Animator>() {
			if (currentLength >= maxComponents) {
				return nullptr;
			}
			else if (GetComponent<Animator>() == nullptr) {
				Animator* animator = new Animator();
				animator->owner = owner;
				animator->m_Id = currentLength;
				currentLength++;
				pushcomponents(*animator);
				return animator;
			}
			return nullptr;
		}

		template <>
		Transform* AddComponent<Transform>() {
			if (currentLength >= maxComponents) {
				return nullptr;
			}
			else if (GetComponent<Transform>() == nullptr) {
				Transform* object = new Transform();
				object->owner = owner;
				object->init();
				object->m_Id = currentLength;
				currentLength++;
				pushcomponents(*object);
				return object;
			}
			return nullptr;
		}
	};
}
#endif // !1