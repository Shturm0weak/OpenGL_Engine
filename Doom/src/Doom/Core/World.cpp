#include "../pch.h"
#include "World.h"
#include "Lua/LuaState.h"
#include "ViewPort.h"
#include "Core/Editor.h"
#include "Objects/Line.h"
#include "Render/Instancing.h"

using namespace Doom;

World& Doom::World::GetInstance()
{
	static World instance; 
	return instance;
}

void Doom::World::UpdateLuaStates()
{
	for(auto l : LuaState::s_LuaStates)
	{
		l->OnUpdate(DeltaTime::s_Deltatime);
	}
}

void Doom::World::StartLuaStates()
{
	for (auto l : LuaState::s_LuaStates)
	{
		l->OnStart();
	}
}

void Doom::World::DeleteAll()
{
	std::lock_guard<std::mutex> lg(m_Mtx);
	for (unsigned int i = 0; i < World::s_GameObjects.size();)
	{
		(World::s_GameObjects[i]->Delete());
	}
	World::s_GameObjects.clear();
	World::s_ColId = 0;
	World::s_ObjId = 0;
}

void Doom::World::DeleteObject(int id)
{
	GameObject* go = World::s_GameObjects[id];
	World::s_GameObjects.erase(World::s_GameObjects.begin() + id);
	World::s_ObjId--;
	unsigned int size = World::s_GameObjects.size();
	if (id != size)
	{
		for (unsigned int i = 0; i < size; i++)
		{
			World::s_GameObjects[i]->m_Id = (i);
			World::s_GameObjects[i]->m_Layer = i;
		}
	}
	unsigned int childsAmount = go->GetChilds().size();
	if (go->GetOwner() != nullptr)
	{
		GameObject* owner = static_cast<GameObject*>(go->GetOwner());
		owner->RemoveChild(go);
	}
	if (go->GetComponent<Irenderer>() != nullptr && go->GetComponent<Irenderer>()->m_RenderType == TYPE_3D)
	{
		go->GetComponent<Renderer3D>()->EraseFromInstancing();
	}
	if (Editor::GetInstance().go == go)
		Editor::GetInstance().go = nullptr;
	go->Delete();
}

GameObject* Doom::World::SelectObject()
{
	std::vector < glm::vec2> p;
	for (unsigned int i = 0; i < GetAmountOfObjects(); i++)
	{
		GameObject* go = static_cast<GameObject*>(World::s_GameObjects[i]);
		SpriteRenderer* sr = static_cast<SpriteRenderer*>(go->m_ComponentManager.GetComponent<Irenderer>());
		p.clear();
		float worldVertexPositions[8];
		sr->GetTransformedVertices(worldVertexPositions);
		p.push_back(glm::vec2(worldVertexPositions[0] + go->GetPosition().x, worldVertexPositions[1] + go->GetPosition().y));
		p.push_back(glm::vec2(worldVertexPositions[2] + go->GetPosition().x, worldVertexPositions[3] + go->GetPosition().y));
		p.push_back(glm::vec2(worldVertexPositions[4] + go->GetPosition().x, worldVertexPositions[5] + go->GetPosition().y));
		p.push_back(glm::vec2(worldVertexPositions[6] + go->GetPosition().x, worldVertexPositions[7] + go->GetPosition().y));
		if (ObjectCollided(p, i)) 
		{
			if (Editor::GetInstance().go != go)
			{
				Editor::GetInstance().go = go;
				return go;
			}
		}
	}
	Editor::GetInstance().go = nullptr;
	return nullptr;

}

void Doom::World::ShutDown()
{
	for (size_t i = 0; i < World::s_GameObjects.size();)
	{
		World::s_GameObjects[i]->Delete();
	}
	World::s_GameObjects.clear();
	World::s_ColId = 0;
	World::s_ObjId = 0;
	for (auto iter = GameObject::s_MemoryPool.begin(); iter != GameObject::s_MemoryPool.end(); iter++)
	{
		delete[](GameObject*)iter->first;
	}
}

void Doom::World::PopBack()
{
	World::s_GameObjects.pop_back();
}

#include "../Rays/Ray3D.h"
#include "../Core/Utils.h"
#include "ImGuizmo/ImGuizmo.h"

GameObject* Doom::World::SelectObject3D()
{
	if (!ImGuizmo::IsOver() && Input::IsMousePressed(Keycode::MOUSE_BUTTON_1) && !Input::IsMouseDown(Keycode::MOUSE_BUTTON_2))
	{
		Ray3D::Hit hit;
		glm::dvec3 pos = Window::GetInstance().GetCamera().GetPosition();
		glm::dvec3 forward = Window::GetInstance().GetCamera().GetMouseDirVec();
		std::map<double, CubeCollider3D*> d = Ray3D::RayCast(pos, forward, &hit, 10000, false);
		for (auto i = d.begin(); i != d.end(); i++)
		{
			if (i->second != nullptr)
			{
				GameObject* go = i->second->GetOwnerOfComponent();
				Transform* tr = go->GetComponent<Transform>();
				glm::mat4 model = tr->m_PosMat4;
				glm::mat4 view = tr->m_ViewMat4;
				glm::mat4 scale = tr->m_ScaleMat4;
				Ray3D::Hit hit1;
				Mesh* mesh = go->m_ComponentManager.GetComponent<Renderer3D>()->m_Mesh;
				//new Line(pos, forward * 1000.f);
				for (uint32_t i = 0; i < mesh->m_VertAttribSize; i += (17 * 3))
				{
					glm::dvec3 a = glm::dvec3(mesh->m_VertAttrib[i + 0], mesh->m_VertAttrib[i + 1], mesh->m_VertAttrib[i + 2]);
					glm::dvec3 b = glm::dvec3(mesh->m_VertAttrib[i + 17 + 0], mesh->m_VertAttrib[i + 17 + 1], mesh->m_VertAttrib[i + 17 + 2]);
					glm::dvec3 c = glm::dvec3(mesh->m_VertAttrib[i + 17 * 2 + 0], mesh->m_VertAttrib[i + 17 * 2 + 1], mesh->m_VertAttrib[i + 17 * 2 + 2]);
					glm::dvec3 n = glm::dvec3(mesh->m_VertAttrib[i + 3], mesh->m_VertAttrib[i + 4], mesh->m_VertAttrib[i + 5]);
					a = glm::dvec3(model * view * scale * glm::vec4(a, 1.0f));
					b = glm::dvec3(model * view * scale * glm::vec4(b, 1.0f));
					c = glm::dvec3(model * view * scale * glm::vec4(c, 1.0f));
					n = glm::dvec3(view * glm::vec4(n, 1.0f));
					if (Ray3D::IntersectTriangle(pos, forward, &hit1, 10000, a, b, c, n))
					{
						//GameObject* go0 = new GameObject("p1", a.x, a.y, a.z);
						//go0->m_ComponentManager.AddComponent<SpriteRenderer>();
						//GameObject* go1 = new GameObject("p2", b.x, b.y, b.z);
						//go1->m_ComponentManager.AddComponent<SpriteRenderer>();
						//GameObject* go2 = new GameObject("p3", c.x, c.y, c.z);
						//go2->m_ComponentManager.AddComponent<SpriteRenderer>();
						/*new Line(a, b);
						new Line(a, c);
						new Line(c, b);*/
#ifndef _IS_GAME_BUILD
						GameObject* prevGo = Editor::GetInstance().go;
						Editor::GetInstance().go = go;
						if (prevGo != nullptr)
						{
							Renderer3D* r3d = prevGo->GetComponent<Renderer3D>();
							if (r3d != nullptr)
								r3d->m_HighLight = false;
						}
						if (go != nullptr)
						{
							Renderer3D* r3d = go->GetComponent<Renderer3D>();
							if (r3d != nullptr)
								r3d->m_HighLight = true;
						}
#endif
						return go;
					}
				}
			}
		}
	}
	return nullptr;
}

unsigned int Doom::World::GetAmountOfObjects()
{
	return World::s_GameObjects.size();
}

bool Doom::World::ObjectCollided(std::vector<glm::vec2>& p, int i)
{
	glm::vec2 MousePos = glm::vec2(ViewPort::GetInstance().GetMousePositionToWorldSpace().x, ViewPort::GetInstance().GetMousePositionToWorldSpace().y);

	for (unsigned int a = 0; a < p.size(); a++)
	{
		{
			int b = (a + 1) % p.size();
			glm::vec2 axisProj = glm::vec2(-(p[b].y - p[a].y), p[b].x - p[a].x);

			float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
			axisProj = { axisProj.x / d, axisProj.y / d };

			float min_r1 = INFINITY, max_r1 = -INFINITY;
			for (int f = 0; f < p.size(); f++)
			{
				float q = (p[f].x * axisProj.x + p[f].y * axisProj.y);
				min_r1 = std::fmin(min_r1, q);
				max_r1 = std::fmax(max_r1, q);
			}

			float min_r2 = INFINITY, max_r2 = -INFINITY;

			float q = (MousePos.x * axisProj.x + MousePos.y * axisProj.y);
			min_r2 = std::fmin(min_r2, q);
			max_r2 = std::fmax(max_r2, q);


			if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
				return false;
		}
	}
	return true;
}