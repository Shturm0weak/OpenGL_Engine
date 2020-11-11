#include "../ECS/ComponentManager.h"
#include "Renderer.h"
#include <iostream>
#include "Batch.h"
#include "../Core/Timer.h"
#include "Line.h"
#include "ViewPort.h"
#include "ParticleSystem.h"
#include "../Core/Editor.h"
#include "Instancing.h"
#include "../Core/World.h"

using namespace Doom;

void Doom::Renderer::Clear(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0f);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void Doom::Renderer::SortTransparentObjects()
{
	std::sort(objects3dTransparent.begin(), objects3dTransparent.end(), [](Renderer3D* r1, Renderer3D* r2) {
		GameObject* go0 = r1->GetOwnerOfComponent();
		GameObject* go1 = r2->GetOwnerOfComponent();
		CubeCollider3D* cc0 = nullptr;
		CubeCollider3D* cc1 = nullptr;
		if (r1->mesh != nullptr)
			cc0 = go0->GetComponentManager()->GetComponent<CubeCollider3D>();
		else
			return false;
		if (r2->mesh != nullptr)
			cc1 = go1->GetComponentManager()->GetComponent<CubeCollider3D>();
		else
			return false;
		glm::vec3 camPos = Window::GetCamera().GetPosition();
		glm::vec3 pos0 = go0->GetPosition();
		glm::vec3 pos1 = go1->GetPosition();
		float d1 = glm::distance(camPos, pos0 + cc0->minP);
		float d2 = glm::distance(camPos, pos0 + cc0->maxP);
		float r1d = 0;
		float r2d = 0;
		if (d1 < d2)
			r1d = d1;
		else
			r1d = d2;
		d1 = glm::distance(camPos, pos1 + cc1->minP);
		d2 = glm::distance(camPos, pos1 + cc1->maxP);
		if (d1 < d2)
			r2d = d1;
		else
			r2d = d2;
		return r1d > r2d;
	});
}

void Doom::Renderer::DeleteAll() {
	for (unsigned int i = 0; i < World::objects.size(); i++)
	{
		delete(World::objects[i]);
	}
	for (unsigned int i = 0; i < Renderer::collision2d.size(); i++)
	{
		delete(Renderer::collision2d[i]);
	}
	Renderer::collision2d.clear();
	World::objects.clear();
	Renderer::col_id = 0;
	Renderer::obj_id = 0;
}

void Doom::Renderer::DeleteObject(int id) {
	GameObject* go = World::objects[id];
	World::objects.erase(World::objects.begin() + id);
	Renderer::obj_id--;
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

GameObject* Doom::Renderer::SelectObject()
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
		if (ObjectCollided(p,i)) {
			if (Editor::Instance()->go != go) {
				Editor::Instance()->go = go;
				return go;
			}
		}
	}
	Editor::Instance()->go = nullptr;
	return nullptr;

}

std::vector<unsigned int> Doom::Renderer::CalculateObjectsVectors()
{
	ObjectsWithNoOwner.clear();
	ObjectsWithOwner.clear();
	for (unsigned int i = 0; i < World::objects.size(); i++)
	{
		if (World::objects[i]->GetOwner() == nullptr) {
			ObjectsWithNoOwner.push_back(World::objects[i]->id);
		}
		else {
			ObjectsWithOwner.push_back(World::objects[i]->id);
		}
	}
	return ObjectsWithNoOwner;
}

void Doom::Renderer::ShutDown()
{
	size_t sizeO = World::objects.size();
	for (size_t i = 0; i < sizeO; i++)
	{
		delete World::objects[i];
	}
	World::objects.clear();
}

void Doom::Renderer::PopBack()
{
	World::objects.pop_back();
}

#include "../Core/Ray3D.h"
#include "../Core/Utils.h"
void Doom::Renderer::SelectObject3D()
{
	if (!Editor::Instance()->gizmo->isHovered && Input::IsMousePressed(Keycode::MOUSE_BUTTON_1) && !Input::IsMouseDown(Keycode::MOUSE_BUTTON_2)) {
		Ray3D::Hit hit;
		glm::vec3 pos = Window::GetCamera().GetPosition();
		glm::vec3 forward = Window::GetCamera().GetMouseDirVec();
		std::map<float, CubeCollider3D*> d = Ray3D::RayCast(pos, forward, &hit, 1000,false);
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
						if (go != Editor::Instance()->gizmo->obj) {
							Editor::Instance()->gizmo->blockFrame = true;
							Editor::Instance()->gizmo->obj = go;
						}
						return;
					}
				}
			}
		}
	}
}

unsigned int Doom::Renderer::GetAmountOfObjects()
{
	return World::objects.size();
}

const char ** Doom::Renderer::GetItems()
{
	delete[] items;
	items = new const char*[GetObjectsWithNoOwnerReference().size()];
	for (unsigned int i = 0; i < GetObjectsWithNoOwnerReference().size(); i++)
	{
		int id = GetObjectsWithNoOwnerReference()[i];
		items[i] = World::objects[id]->name.c_str();

	}
	return items;
}

GameObject * Doom::Renderer::CreateGameObject()
{
	GameObject * go = new GameObject("Unnamed", 0, 0);
	return go;
}

bool Doom::Renderer::ObjectCollided(std::vector<glm::vec2>& p,int i)
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

void Renderer::Render() {
	if (Window::GetCamera().type == Camera::CameraTypes::ORTHOGRAPHIC) {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		Render2DObjects();
		RenderCollision();
		RenderLines();
		RenderText();
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		return;
	}			
	
	RenderCollision();
	Render3DObjects();
	RenderLines();
	RenderCollision3D();
	Render2DObjects();
	RenderTransparent();
	if(Editor::Instance()->gizmo != nullptr)
		Editor::Instance()->gizmo->Render();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	RenderText();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void Doom::Renderer::Render2DObjects()
{
	{
		size_t size = Renderer::objects2d.size();
		if (size > 0)
		{
			Batch* batch = Batch::GetInstance();
			batch->BeginGameObjects();
			for (size_t i = 0; i < size;i++) {
				SpriteRenderer* r = Renderer::objects2d[i];
				if (r->GetOwnerOfComponent()->Enable == true)// && sqrt(pow((go->position.x - Window::GetCamera().GetPosition().x), 2) + pow((go->position.y - Window::GetCamera().GetPosition().y), 2)) < 50 * Window::GetCamera().GetZoomLevel())
				{
					r->Render();
				}
			}
			size_t particleSize = Particle::particles.size();
			if (particleSize > 0) {

				for (size_t i = 0; i < particleSize; i++)
				{
					Particle* p = Particle::particles[i];
					if (p->Enable)
						Batch::GetInstance()->Submit(p->pos, p->view, p->color, glm::vec2(p->scaleX, p->scaleY), p->texture);
				}
			}
			batch->EndGameObjects();
		}
		
	}

	Batch::GetInstance()->flushGameObjects(Batch::GetInstance()->BasicShader);
}

void Doom::Renderer::Render3DObjects()
{
	if(PolygonMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	for each (Renderer3D* r in Renderer::objects3d)
	{
		if (r->GetOwnerOfComponent()->Enable == true)
		{
			r->Render();
		}
	}

	Instancing::Instance()->Render();

	if (PolygonMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Doom::Renderer::BakeShadows()
{
	for each (Renderer3D* r in Renderer::objects3d)
	{
		if (r->GetOwnerOfComponent()->Enable == true)
		{
			r->BakeShadows();
		}
	}
	for each (Renderer3D* r in Renderer::objects3dTransparent)
	{
		if (r->GetOwnerOfComponent()->Enable == true)
		{
			r->BakeShadows();
		}
	}
}

void Doom::Renderer::UpdateLightSpaceMatrices()
{
	for (DirectionalLight* light : DirectionalLight::dirLights)
	{
		light->UpdateLightMatrix();
	}
}

void Doom::Renderer::RenderCollision3D()
{
	size_t size = CubeCollider3D::colliders.size();
	if (size > 0) {
		for (size_t i = 0; i < size; i++)
		{
			if (!CubeCollider3D::colliders[i]->isBoundingBox && RectangleCollider2D::IsVisible)
				CubeCollider3D::colliders[i]->Render();
		}
	}
	if (size > 0) {
		for (size_t i = 0; i < size; i++)
		{
			if (CubeCollider3D::colliders[i]->isBoundingBox && Editor::Instance()->isBoundingBoxesVisible)
				CubeCollider3D::colliders[i]->Render();
		}
	}
	size = SphereCollider::spheres.size();
	if (size > 0) {
		for (size_t i = 0; i < size; i++)
		{
			if (RectangleCollider2D::IsVisible)
				SphereCollider::spheres[i]->Render();
		}
	}
}

void Doom::Renderer::RenderLines()
{
	Batch::GetInstance()->BeginLines();
	uint32_t size = Line::lines.size();
	for (uint32_t i = 0; i < size; i++)
	{
		if (Line::lines[i]->Enable) {
			Batch::GetInstance()->Submit(*Line::lines[i]);
		}
	}
	if (Editor::Instance()->drawNormals) {
		size = Editor::Instance()->normals.size();
		for (uint32_t i = 0; i < size; i++)
		{
			Batch::GetInstance()->Submit(*Editor::Instance()->normals[i]);
		}
	}
	Batch::GetInstance()->EndLines();
	Batch::GetInstance()->flushLines(Batch::GetInstance()->LineShader);
}

void Doom::Renderer::RenderText() {
	Batch::GetInstance()->flushText(Batch::GetInstance()->TextShader);
}

void Doom::Renderer::RenderTransparent()
{
	for each (Renderer3D* r in Renderer::objects3dTransparent)
	{
		if (r->GetOwnerOfComponent()->Enable == true)
		{
			r->Render();
		}
	}
}

void Doom::Renderer::RenderCollision(){
	if (RectangleCollider2D::IsVisible == true) {
		Batch::GetInstance()->BeginGameObjects();
		for (unsigned int i = 0; i < Renderer::collision2d.size(); i++) {
			RectangleCollider2D* col = Renderer::collision2d[i];
			if (col != nullptr && col->Enable == true) {
				Batch::GetInstance()->Submit(*col);
			}
		}
		Batch::GetInstance()->EndGameObjects();
		Batch::GetInstance()->flushCollision(Batch::GetInstance()->CollisionShader);
	}
}