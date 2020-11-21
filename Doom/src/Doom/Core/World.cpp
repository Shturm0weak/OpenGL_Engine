#include "../pch.h"
#include "World.h"
#include "Lua/LuaState.h"
#include "ViewPort.h"
#include "Core/Editor.h"

using namespace Doom;

void Doom::World::ProccessLuaStates()
{
	for(auto l : LuaState::luaStates)
	{
		l->OnUpdate(DeltaTime::deltatime);
	}
}

void Doom::World::StartLuaStates()
{
	for (auto l : LuaState::luaStates)
	{
		l->OnStart();
	}
}

void Doom::World::DeleteAll() {
	for (unsigned int i = 0; i < World::objects.size(); i++)
	{
		delete(World::objects[i]);
	}
	World::objects.clear();
	World::col_id = 0;
	World::obj_id = 0;
}

void Doom::World::DeleteObject(int id) {
	GameObject* go = World::objects[id];
	World::objects.erase(World::objects.begin() + id);
	World::obj_id--;
	unsigned int size = World::objects.size();
	if (id != size) {
		for (unsigned int i = 0; i < size; i++)
		{
			World::objects[i]->id = (i);
			World::objects[i]->layer = i;
		}
	}
	unsigned int childsAmount = go->GetChilds().size();
	for (unsigned int i = 0; i < childsAmount; i++)
	{
		GameObject* child = static_cast<GameObject*>(go->GetChilds()[i]);
		child->SetOwner(nullptr);
	}
	if (go->GetOwner() != nullptr) {
		GameObject* owner = static_cast<GameObject*>(go->GetOwner());
		owner->RemoveChild(go);
	}
	if (go->GetComponent<Irenderer>() != nullptr && go->GetComponent<Irenderer>()->renderType == TYPE_3D) {
		go->GetComponent<Renderer3D>()->EraseFromInstancing();
	}
	delete go;
}

GameObject* Doom::World::SelectObject()
{
	std::vector < glm::vec2> p;
	for (unsigned int i = 0; i < GetAmountOfObjects(); i++)
	{
		GameObject* go = static_cast<GameObject*>(World::objects[i]);
		SpriteRenderer* sr = static_cast<SpriteRenderer*>(go->GetComponentManager()->GetComponent<Irenderer>());
		p.clear();
		p.push_back(glm::vec2(sr->WorldVertexPositions[0] + go->GetPosition().x, sr->WorldVertexPositions[1] + go->GetPosition().y));
		p.push_back(glm::vec2(sr->WorldVertexPositions[2] + go->GetPosition().x, sr->WorldVertexPositions[3] + go->GetPosition().y));
		p.push_back(glm::vec2(sr->WorldVertexPositions[4] + go->GetPosition().x, sr->WorldVertexPositions[5] + go->GetPosition().y));
		p.push_back(glm::vec2(sr->WorldVertexPositions[6] + go->GetPosition().x, sr->WorldVertexPositions[7] + go->GetPosition().y));
		if (ObjectCollided(p, i)) {
			if (Editor::GetInstance()->go != go) {
				Editor::GetInstance()->go = go;
				return go;
			}
		}
	}
	Editor::GetInstance()->go = nullptr;
	return nullptr;

}

void Doom::World::ShutDown()
{
	size_t sizeO = World::objects.size();
	for (size_t i = 0; i < sizeO; i++)
	{
		delete World::objects[i];
	}
	World::objects.clear();
}

void Doom::World::PopBack()
{
	World::objects.pop_back();
}

#include "../Rays/Ray3D.h"
#include "../Core/Utils.h"

void Doom::World::SelectObject3D()
{
	if (!Editor::GetInstance()->gizmo->isHovered && Input::IsMousePressed(Keycode::MOUSE_BUTTON_1) && !Input::IsMouseDown(Keycode::MOUSE_BUTTON_2)) {
		Ray3D::Hit hit;
		glm::vec3 pos = Window::GetCamera().GetPosition();
		glm::vec3 forward = Window::GetCamera().GetMouseDirVec();
		std::map<float, CubeCollider3D*> d = Ray3D::RayCast(pos, forward, &hit, 1000, false);
		for (auto i = d.begin(); i != d.end(); i++)
		{
			if (i->second != nullptr) {
				GameObject* go = i->second->GetOwnerOfComponent();
				Transform* tr = go->GetComponent<Transform>();
				glm::mat4 model = tr->pos;
				glm::mat4 view = tr->view;
				glm::mat4 scale = tr->scale;
				Ray3D::Hit hit1;
				Mesh* mesh = go->GetComponentManager()->GetComponent<Renderer3D>()->mesh;
				//new Line(pos, forward * 1000.f);
				for (uint32_t i = 0; i < mesh->meshSize; i += (14 * 3))
				{
					glm::vec3 a = glm::vec3(mesh->mesh[i + 0], mesh->mesh[i + 1], mesh->mesh[i + 2]);
					glm::vec3 b = glm::vec3(mesh->mesh[i + 14 + 0], mesh->mesh[i + 14 + 1], mesh->mesh[i + 14 + 2]);
					glm::vec3 c = glm::vec3(mesh->mesh[i + 28 + 0], mesh->mesh[i + 28 + 1], mesh->mesh[i + 28 + 2]);
					glm::vec3 n = glm::vec3(mesh->mesh[i + 3], mesh->mesh[i + 4], mesh->mesh[i + 5]);
					a = glm::vec3(model * view * scale * glm::vec4(a, 1.0f));
					b = glm::vec3(model * view * scale * glm::vec4(b, 1.0f));
					c = glm::vec3(model * view * scale * glm::vec4(c, 1.0f));
					n = glm::vec3(view * glm::vec4(n, 1.0f));
					if (Ray3D::IntersectTriangle(pos, forward, &hit1, 1000, a, b, c, n)) {
						//GameObject* go0 = new GameObject("p1", a.x, a.y, a.z);
						//go0->GetComponentManager()->AddComponent<SpriteRenderer>();
						//GameObject* go1 = new GameObject("p2", b.x, b.y, b.z);
						//go1->GetComponentManager()->AddComponent<SpriteRenderer>();
						//GameObject* go2 = new GameObject("p3", c.x, c.y, c.z);
						//go2->GetComponentManager()->AddComponent<SpriteRenderer>();
						/*new Line(a, b);
						new Line(a, c);
						new Line(c, b);*/
						if (go != Editor::GetInstance()->gizmo->obj) {
							Editor::GetInstance()->gizmo->blockFrame = true;
							Editor::GetInstance()->gizmo->obj = go;
						}
						return;
					}
				}
			}
		}
	}
}

unsigned int Doom::World::GetAmountOfObjects()
{
	return World::objects.size();
}

GameObject* Doom::World::CreateGameObject()
{
	GameObject* go = new GameObject("Unnamed", 0, 0);
	return go;
}

bool Doom::World::ObjectCollided(std::vector<glm::vec2>& p, int i)
{
	glm::vec2 MousePos = glm::vec2(ViewPort::GetInstance()->GetMousePositionToWorldSpace().x, ViewPort::GetInstance()->GetMousePositionToWorldSpace().y);

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