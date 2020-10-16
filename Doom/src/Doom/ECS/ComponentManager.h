#pragma once
#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Component.h"
#include "../Render/Renderer.h"
#include "../Components/SpriteRenderer.h"
#include "../Components/Render3D.h"
#include "../Components/Transform.h"
#include "../Components/Animator.h"
#include "../Components/RectangleCollider2D.h"
#include "../Components/CubeCollider.h"
#include "../Core/ColoredOutput.h"
#include "../Components/PointLight.h"
#include "../Components/DirectionalLight.h"

namespace Doom {

	enum class ComponentTypes {
		TRANSFORM,
		RENDER,
		CUBECOLLIDER3D,
		COLLISION,
		ANIMATOR,
		POINTLIGHT,
		DIRECTIONALLIGHT,
	};

	class DOOM_API ComponentManager {
		GameObject* owner = nullptr;
		std::string owner_name;
		const char** namesOfMembers = nullptr;
		std::vector <Component*> components;

		friend class Component;

	public:
		~ComponentManager() { 
			delete[] namesOfMembers;
			RemoveComponent<RectangleCollider2D>();
			RemoveComponent<Transform>();
			RemoveComponent<Animator>();
			RemoveComponent<Irenderer>();
			RemoveComponent<CubeCollider3D>();
		}
		ComponentManager(GameObject* owner, std::string& owner_name) {
			this->owner = owner;
			this->owner_name = owner_name;
		}

		int GetAmountOfComponents() { return components.size(); }

		const char** GetItems() {
			if (namesOfMembers != nullptr)
				delete[] namesOfMembers;
			namesOfMembers = new const char*[components.size()];
			for (unsigned int i = 0; i < components.size(); i++)
			{
				namesOfMembers[i] = std::to_string((int)components[i]->GetComponentType()).c_str();
			}
			return namesOfMembers;
		}

		template <class T >
		void RemoveComponent() {
			return;
		}

		template <>
		void RemoveComponent<RectangleCollider2D>() {
			RectangleCollider2D* col = nullptr;
			col = GetComponent<RectangleCollider2D>();
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
		void RemoveComponent<PointLight>() {
			PointLight* col = nullptr;
			col = GetComponent<PointLight>();
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
		void RemoveComponent<DirectionalLight>() {
			DirectionalLight* col = nullptr;
			col = GetComponent<DirectionalLight>();
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
		void RemoveComponent<CubeCollider3D>() {
			CubeCollider3D* col = nullptr;
			col = GetComponent<CubeCollider3D>();
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
		void RemoveComponent<Irenderer>() {
			Irenderer* col = nullptr;
			col = GetComponent<Irenderer>();
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

			if(col->renderType == "2D"){
				auto iter = std::find(Renderer::objects2d.begin(), Renderer::objects2d.end(), col);
				if (iter != Renderer::objects2d.end()) {
					Renderer::objects2d.erase(iter);
				}
			}
			else if (col->renderType == "3D") {
				auto iter = std::find(Renderer::objects3d.begin(), Renderer::objects3d.end(), col);
				if (iter != Renderer::objects3d.end()) {
					Renderer::objects3d.erase(iter);
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
		SpriteRenderer* GetComponent() {
			return static_cast<SpriteRenderer*>(GetComponent<Irenderer>());
		}

		template<>
		Renderer3D* GetComponent() {
			return static_cast<Renderer3D*>(GetComponent<Irenderer>());
		}

		template<>
		RectangleCollider2D* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				if (components[i]->GetComponentType() == ComponentTypes::COLLISION) {
					return (RectangleCollider2D*)components[i];
				}
			}
			return nullptr;
		}

		template<>
		Irenderer* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				if (components[i]->GetComponentType() == ComponentTypes::RENDER) {
					return (Irenderer*)components[i];
				}
			}
			return nullptr;
		}

		template<>
		DirectionalLight* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				if (components[i]->GetComponentType() == ComponentTypes::DIRECTIONALLIGHT) {
					return (DirectionalLight*)components[i];
				}
			}
			return nullptr;
		}

		template<>
		PointLight* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				if (components[i]->GetComponentType() == ComponentTypes::POINTLIGHT) {
					return (PointLight*)components[i];
				}
			}
			return nullptr;
		}

		template<>
		Animator* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				if (components[i]->GetComponentType() == ComponentTypes::ANIMATOR) {
					return (Animator*)components[i];
				}
			}
			return nullptr;
		}

		template<>
		CubeCollider3D* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				if (components[i]->GetComponentType() == ComponentTypes::CUBECOLLIDER3D) {
					return (CubeCollider3D*)components[i];
				}
			}
			return nullptr;
		}

		template<>
		Transform* GetComponent() {
			for (unsigned int i = 0; i < components.size(); i++)
			{
				Component* comp = components[i];
				if (comp->GetComponentType() == ComponentTypes::TRANSFORM) {
					return (Transform*)comp;
				}
			}
			return nullptr;
		}

		template<class T>
		T* AddComponent() {

		}

		template <>
		RectangleCollider2D* AddComponent<RectangleCollider2D>() {
			if (GetComponent<RectangleCollider2D>() == nullptr) {
				RectangleCollider2D* object = new RectangleCollider2D(owner);
				object->owner = (this->owner);
				object->m_Id = components.size();
				components.push_back(object);
#ifdef _DEBUG
				std::cout << "Added component of type <Collision> for gameobject: " << owner_name << std::endl;
#endif
				return object;
			}
			return nullptr;
		}

		template <>
		PointLight* AddComponent<PointLight>() {
			if (GetComponent<PointLight>() == nullptr) {
				PointLight* object = new PointLight();
				object->owner = (this->owner);
				object->m_Id = components.size();
				components.push_back(object);
#ifdef _DEBUG
				std::cout << "Added component of type <PointLight> for gameobject: " << owner_name << std::endl;
#endif
				return object;
			}
			return nullptr;
		}

		template <>
		DirectionalLight* AddComponent<DirectionalLight>() {
			if (GetComponent<DirectionalLight>() == nullptr) {
				DirectionalLight* object = new DirectionalLight();
				object->owner = (this->owner);
				object->m_Id = components.size();
				components.push_back(object);
#ifdef _DEBUG
				std::cout << "Added component of type <DirectionalLight> for gameobject: " << owner_name << std::endl;
#endif
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
#ifdef _DEBUG
				std::cout << "Added component of type <SpriteRenderer> for gameobject: " << owner_name << std::endl;
#endif
				return object;
			}
			return nullptr;
		}

		template <>
		Renderer3D* AddComponent<Renderer3D>() {
			if (GetComponent<Renderer3D>() == nullptr) {
				Renderer3D* object = new Renderer3D(owner);
				object->owner = (this->owner);
				object->m_Id = components.size();
				components.push_back(object);
#ifdef _DEBUG
				std::cout << "Added component of type <Renderer3D> for gameobject: " << owner_name << std::endl;
#endif
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
#ifdef _DEBUG
				std::cout << "Added component of type <Animator> for gameobject: " << owner_name << std::endl;
#endif
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
#ifdef _DEBUG
				std::cout << "Added component of type <Transform> for gameobject: " << owner_name << std::endl;
#endif
				return object;
			}
			return nullptr;
		}

		template <>
		CubeCollider3D* AddComponent<CubeCollider3D>() {
			if (GetComponent<CubeCollider3D>() == nullptr) {
				CubeCollider3D* object = new CubeCollider3D();
				object->owner = (this->owner);
				object->m_Id = components.size();
				components.push_back(object);
#ifdef _DEBUG
				std::cout << "Added component of type <CubeCollider3D> for gameobject: " << owner_name << std::endl;
#endif
				return object;
			}
			return nullptr;
		}
	};
}
#endif // !1