#include "../ECS/ComponentManager.h"
#include "Renderer.h"
#include <iostream>
#include "Batch.h"
#include "../Core/Timer.h"
#include "../Objects/Line.h"
#include "../Core/ViewPort.h"
#include "../Objects/ParticleSystem.h"
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
		if (r1->m_Mesh != nullptr)
			cc0 = go0->GetComponentManager()->GetComponent<CubeCollider3D>();
		else
			return false;
		if (r2->m_Mesh != nullptr)
			cc1 = go1->GetComponentManager()->GetComponent<CubeCollider3D>();
		else
			return false;
		glm::vec3 camPos = Window::GetCamera().GetPosition();
		glm::vec3 pos0 = go0->GetPosition();
		glm::vec3 pos1 = go1->GetPosition();
		float d1 = glm::distance(camPos, pos0 + cc0->m_MinP);
		float d2 = glm::distance(camPos, pos0 + cc0->m_MaxP);
		float r1d = 0;
		float r2d = 0;
		if (d1 < d2)
			r1d = d1;
		else
			r1d = d2;
		d1 = glm::distance(camPos, pos1 + cc1->m_MinP);
		d2 = glm::distance(camPos, pos1 + cc1->m_MaxP);
		if (d1 < d2)
			r2d = d1;
		else
			r2d = d2;
		return r1d > r2d;
	});
}

void Renderer::Render() {
	if (Window::GetCamera().m_Type == Camera::CameraTypes::ORTHOGRAPHIC) {
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
	if(Editor::GetInstance()->gizmo != nullptr)
		Editor::GetInstance()->gizmo->Render();
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
				if (r->GetOwnerOfComponent()->m_Enable == true)// && sqrt(pow((go->position.x - Window::GetCamera().GetPosition().x), 2) + pow((go->position.y - Window::GetCamera().GetPosition().y), 2)) < 50 * Window::GetCamera().GetZoomLevel())
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
		if (r->GetOwnerOfComponent()->m_Enable == true)
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
		if (r->GetOwnerOfComponent()->m_Enable == true)
		{
			r->BakeShadows();
		}
	}
	for each (Renderer3D* r in Renderer::objects3dTransparent)
	{
		if (r->GetOwnerOfComponent()->m_Enable == true)
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
			if (!CubeCollider3D::colliders[i]->m_IsBoundingBox && RectangleCollider2D::IsVisible)
				CubeCollider3D::colliders[i]->Render();
		}
	}
	if (size > 0) {
		for (size_t i = 0; i < size; i++)
		{
			if (CubeCollider3D::colliders[i]->m_IsBoundingBox && Editor::GetInstance()->isBoundingBoxesVisible)
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
	if (Editor::GetInstance()->drawNormals) {
		size = Editor::GetInstance()->normals.size();
		for (uint32_t i = 0; i < size; i++)
		{
			Batch::GetInstance()->Submit(*Editor::GetInstance()->normals[i]);
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
		if (r->GetOwnerOfComponent()->m_Enable == true)
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